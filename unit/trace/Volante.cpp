#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "Volante.h"
#include "BatteryMonitor.h"
#include "Timer.h"
#include "Logger.h"
#include "common.h"

#define power_u_limit (4)
#define power_l_limit (-4)
#define steer_u_limit (5)
#define steer_l_limit (-5)

Volante::Volante(Motor &left, Motor &right)
		:leftMotor(left), rightMotor(right) {
	integral    = 0;
	steer_angle = 0;
	error_pre   = 0;
	error_cur   = 0;
	power = 0;
	motor_left_power = 0;
	motor_right_power = 0;
	leftMotor.reset();
	rightMotor.reset();
	ang_Tl = 0;
	ang_Tr = 0;
	ang_Tl_pre = 0;
	ang_Tr_pre = 0;
	ang_Tl_cur = 0;
	ang_Tr_cur = 0;
	l.set(0.3,0,0);
	v.set(100,0,0);
	r.set(0.2,0,0);
	speed_cur = 0;
	speed_err = 0;
	speed_integral = 0;
	power_err = 0;
	radius_cur = 0;
	radius_err = 0;
	radius_integral = 0;
	batteryMonitor = new BatteryMonitor();
	batteryMonitor->measure();
	Vbat = (batteryMonitor->get_voltage());
	Volante::readIniFile();
	const_speed = PI*WHEEL_RADIUS/360;
	tgtSpeed = 0.45;
	tgtRadius = 0;
	flag_reverse = false;
	max_steer_angle = 200;
	time_err = 0;
}

void Volante::set_pid(Pid &pid) {
	l.Kp = pid.Kp;
	l.Ki = pid.Ki;
	l.Kd = pid.Kd;
}

void Volante::set_speed(float spd) {
	speed_tgt = spd;
}

void Volante::readIniFile(){

	FILE *fp;
	
	if( (fp = fopen("conf.ini","r")) == NULL ){
	
		print_new_linef("ERROR: initial file can't open");
	
	} else {
	
		char line[MAX_STR]={0};
		char *key  = NULL;
		char *num  = NULL;
		float val = 0;
		while(fgets(line, sizeof(line), fp)){
			key = strtok(line, "=");
			num = strtok(NULL, "=");
			val = atof(num);
			if(!strcmp(key,"vP")) {
				v.Kp = val;
			} else if(!strcmp(key,"vI")) {
				v.Ki = val;
			} else if(!strcmp(key,"rP")) {
				r.Kp = val;
			} else if(!strcmp(key,"rI")) {
				r.Ki = val;
			} else if(!strcmp(key,"radius")) {
				tgtRadius = (int)val;
			}
		}
		fclose(fp);
	}
}

float Volante::getSpeed(){
	return tgtSpeed;
}

int Volante::getRadius(){
	return tgtRadius;
}

void Volante::reverse_steer(bool mode) {
	flag_reverse = mode;
}

// 各タイヤの角度を取得
int Volante::getDegTire() {
	ang_Tl_pre = ang_Tl_cur;
	ang_Tr_pre = ang_Tr_cur;
	ang_Tl_cur = leftMotor.getCount();
	ang_Tr_cur = rightMotor.getCount();
	ang_Tl = ang_Tl_cur - ang_Tl_pre;
	ang_Tr = ang_Tr_cur - ang_Tr_pre;
	
//	print_new_linef("ang_Tl %d", ang_Tl);
//	print_new_linef("ang_Tr %d", ang_Tr);
	
	return(0);
}

// 現在の曲率を計算
int Volante::calcRadius(){
	
	Volante::getDegTire();
	
	if (ang_Tl == 0 && ang_Tr == 0){
		radius_cur = 0;
	} else if(ang_Tl == -ang_Tr) {
		if (ang_Tl > ang_Tr) { radius_cur =  2; }
		if (ang_Tl < ang_Tr) { radius_cur = -2; }
	} else {
		radius_cur = (ang_Tl - ang_Tr)/(ang_Tl + ang_Tr) * 2/TRED_WIDTH;
	}
	return(0);
}



// PIDsteer
int Volante::steerPID(float err){
	integral += (error_cur = err);
	steer_angle = l.Kp * err + l.Ki * integral + l.Kd * (error_cur - error_pre);
	if (flag_reverse) steer_angle = -steer_angle;
	error_pre = error_cur;
	
	Volante::limit_steer();
	
//	print_new_line("steer_angle %f", steer_angle);
//	print_new_line("steer_rev %d", flag_reverse);
	
	return(0);
}

// 固定値steer
int Volante::steer(int angle){
	steer_angle = angle;
	Volante::limit_steer();
	
	return(0);
}

// R指定steer
// TODO: ステアリングの反転制御の対応ができていない
int Volante::steerR(int radius_tgt){
	
	Volante::calcRadius();
	
	// steerの偏差を計算
	radius_err = 1/radius_tgt - radius_cur;
	radius_integral += radius_err;
	steer_err = radius_err * r.Kp + radius_integral * r.Ki;
//	print_new_linef("curve_err %f", steer_err);
//	print_new_linef("steer_err %f", steer_err);
	// steer_angleを補正
	steer_angle += steer_err;
	
	// steer_angleの上下限を設定
	Volante::limit_steer();
	
//	print_new_linef("steer_angle %f", steer_angle);
	
	return(0);
}

// steer_angleの上下限を設定
int Volante::limit_steer() {

	if (max_steer_angle > 200) { max_steer_angle = 200;}
	if (steer_angle >  max_steer_angle) {steer_angle =  max_steer_angle;}
	if (steer_angle < -max_steer_angle) {steer_angle = -max_steer_angle;}
	
	return(0);
}

// max_steerの値を書き換える
int Volante::setMaxSteer(int MaxStr) {
	max_steer_angle = MaxStr;
	return(0);
}


// steerの値をとる
float Volante::getSteer() {
	return (steer_angle);
}

// 現在の車速を計算
float Volante::calc_velocity() {
	
	static ulong_t cur_time=0;
	static ulong_t pre_time=0;
	
	// 現在の時刻を取得
	get_tim(&cur_time);
	
	Volante::getDegTire();
	
	if( (time_err = cur_time - pre_time) != 0 ) {
		speed_cur = const_speed * (ang_Tl + ang_Tr)/time_err;
		pre_time  = cur_time;
	}
	return(speed_cur);
}

// 車速追従PWM制御
float Volante::calc_speed_pid() {
	
	// 現在の車速を計算
	Volante::calc_velocity();
	
	// Powerの偏差を計算
	speed_err = speed_tgt - speed_cur;
	speed_integral += speed_err;
	power_err = (speed_err * v.Kp + speed_integral * v.Ki) / Vbat;
//	print_new_line("power_err %f", power_err);
	// Powerの変化量を制限
	if(power_err > power_u_limit) { power_err = power_u_limit; }
	if(power_err < power_l_limit) { power_err = power_l_limit; }
	
	// powerを決定
	power += power_err;
	
	// powerの上下限を設定
	if(power > 100)  { power =  100; }
	if(power < -100) { power = -100; }
	
//	print_new_line("power %f", power);
	return(power);
}


// 左右輪のPWMを指定
int Volante::drive() {
	
	// steerから左右輪のpowerを計算
	motor_left_power  = (1 + steer_angle/100) * power;
	motor_right_power = (1 - steer_angle/100) * power;
	if (power < motor_left_power)  { motor_left_power  = power; }
	if (power < motor_right_power) { motor_right_power = power; }
	Volante::setPWM(motor_left_power, motor_right_power );
	
	return(0);
}

#define LEFT_MOTOR  (EV3_PORT_D)
#define RIGHT_MOTOR (EV3_PORT_A)

int Volante::rotate_wheel_r(int deg, int power, bool block) {
	return ev3_motor_rotate(RIGHT_MOTOR, deg, power, block);
}

int Volante::rotate_wheel_l(int deg, int power, bool block) {
	return ev3_motor_rotate(LEFT_MOTOR, deg, power, block);
}

int Volante::get_angle_wheel_l() {
	return leftMotor.getCount();
}

int Volante::get_angle_wheel_r() {
	return rightMotor.getCount();
}

int Volante::setPWM(float duty_left, float duty_right ) {
	
	leftMotor.setPWM(duty_left);
	rightMotor.setPWM(duty_right);
	
	print_new_line("speed_tgt %f", speed_tgt);
	print_new_line("speed_cur %f", speed_cur);
	print_new_line("power %f",power);
	print_new_line("motor_power L %f, R %f", duty_left, duty_right);
	
	return(0);
}

int Volante::reset_power() {
	print_new_line("motor_power reset");
	power = 0;
	power_err = 0;
	speed_cur = 0;
	return(0);
}

int Volante::stop(){
	print_new_line("motor stop");
	leftMotor.stop();
	rightMotor.stop();
	return(0);
}

float Volante::getPower() {
	return(power);
}

int Volante::setPower(float power_ini) {
	power = power_ini;
	return(0);
}

float Volante::getPower_r() {
	return(motor_right_power);
}

float Volante::getPower_l() {
	return(motor_left_power);
}
