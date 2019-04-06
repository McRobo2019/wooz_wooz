
#ifndef VOLANTE_H_
#define VOLANTE_H_

#include "ev3api.h"
#include "Motor.h"
#include "BatteryMonitor.h"
#include "common.h"

using namespace ev3api;

class Pid {

	public:
		Pid() { Kp=Ki=Kd=0; }
		Pid(float kp, float ki, float kd): Kp(kp), Ki(ki), Kd(kd) {;}
		void set(float kp, float ki, float kd){
			Kp = kp; Ki = ki; Kd = kd;
		}
		float Kp;
		float Ki;
		float Kd;
};

// ボランチクラス
class Volante {

	public:
		explicit Volante(Motor &left, Motor &right);
		float getSpeed();
		int   getRadius();
		int   getDegTire();
		int   steerPID(float err);
		int   steer(int angle);
		int   steerR(int radius_tgt);
		float calc_speed_pid();
		void  set_pid(Pid &pid);
		void  set_speed(float spd);
		int   drive();
		int   stop();
		void  reverse_steer(bool mode);
		int   rotate_wheel_r(int deg, int power, bool block);
		int   rotate_wheel_l(int deg, int power, bool block);
		int   get_angle_wheel_r();
		int   get_angle_wheel_l();
		int   setPWM(float duty_left, float duty_right);
		float getSteer();
		int   setMaxSteer(int MaxStr);
		int   reset_power();
		float getPower();
		int   setPower(float power_ini);
		float getPower_r();
		float getPower_l();

	private:
		void  readIniFile();
		float calc_velocity();
		int   calcRadius();
		int limit_steer();
		Motor& leftMotor;
		Motor& rightMotor;
		BatteryMonitor* batteryMonitor;
		float motor_left_power;
		float motor_right_power;
		float integral;
		float error_cur;
		float error_pre;
		float power;
		float steer_angle;
		Pid l;
		Pid v;
		Pid r;
		float tgtSpeed;		// ファイルから読み込む目標車速
		int tgtRadius;		// ファイルから読み込む目標半径
		int ang_Tl;			// 左タイヤ角度
		int ang_Tr;			// 右タイヤ角度
		int ang_Tl_pre;		// 前回の左タイヤ角度
		int ang_Tr_pre;		// 前回の右タイヤ角度
		int ang_Tl_cur;		// 左タイヤのセンサ値
		int ang_Tr_cur;		// 右タイヤのセンサ値
		float Vbat;				// バッテリ電圧
		float speed_tgt;		// 現在の車速
		float speed_cur;		// 現在の車速
		float speed_err;		// 車速の偏差
		float speed_integral;	// 車速偏差の合計
		float power_err;		// Dutyの偏差
		float radius_cur;		// 現在の回転半径
		float radius_err;		// 回転半径の偏差
		float radius_integral;	// 回転半径の偏差の合計
		float steer_err;		// steer_angleの偏差
		float const_speed;      // ?
		bool flag_reverse;      // ステアリングの反転フラグ(真:反転, 偽:正転)
		int max_steer_angle;
		ulong_t time_err;
};

#endif // VOLANTE_H_
