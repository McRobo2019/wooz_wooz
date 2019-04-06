#include "ColorIdentify.h"
#include "Logger.h"

#define ForwardAngle_l 72
#define UnderAngle 14
#define defColorNum 0

ColorIdentify::ColorIdentify(Volante *vlnt, StopWatch *watch, Motor& arm, ColorSensor& sensor)
				: Motion(vlnt, watch), armMotor(arm), colorSensor(sensor){
	color = (colorid_t)0;
	armMotor.reset();
	setArmAngle(UnderAngle);
	print_new_line("armAngle %d", armMotor.getCount());
	power = 0;
	color_num = defColorNum;
}

int ColorIdentify::drive() {
	print_new_line("color identify start");
	volante->stop();
	volante->reset_power();
	if( type == 0 ) {
		setArmAngle(UnderAngle);
		getColorNum();
		// 床色を青と間違う恐れがあるため青の場合、
		// raw値を使って緑と区別する
		if( (getColorNum()==2) && (getBlueVal()<150) ) {
			color_num = 3;
		}
	} else {
		for(int i=0; i<3; i++) {
			setArmAngle(ForwardAngle_l + 5*i);
			palette[i] = getColorNum();
		}
		setArmAngle(UnderAngle);
		if(palette[0] == palette[1]) {
			color_num = palette[0];
		} else if(palette[1] == palette[2]) {
			color_num = palette[1];
		} else if(palette[2] == palette[0]) {
			color_num = palette[2];
		} else {
			color_num = defColorNum;
		}
	}
	colorSensor.getBrightness();
	print_new_line("color %d",color_num);
	print_new_line("color identify end");
	return(color_num);
}

int ColorIdentify::getColor() {
	return(color_num);
}

int ColorIdentify::getColorNum() {
	
	color = colorSensor.getColorNumber();
	tslp_tsk(400);
	color_num = (int)color;
	print_new_line("color %d", color_num);
	return color_num;
}

int ColorIdentify::getBlueVal() {
	colorSensor.getRawColor(rgb_val);
	blue_val = (int)rgb_val.b;
	return(blue_val);
}

int ColorIdentify::getAngle() { 
	return armMotor.getCount();
}

// 腕を指定の角度に動かす
int ColorIdentify::setArmAngle(int tgtAngle) {

	// tgtAngleの範囲を制限
	if( tgtAngle < 0   ) { tgtAngle = 0  ; }
	if( tgtAngle > 120 ) { tgtAngle = 120; }
	
	// 腕が真下より引き側の時は早く、Armを上げる際はゆっくり。おろす際は素早く
	if( ColorIdentify::getAngle() < UnderAngle-5 )  { power = 70; }
	else if( tgtAngle > ColorIdentify::getAngle() ) { power = 10; }
	else { power = 70; }
	
	ev3_motor_rotate(EV3_PORT_B, (tgtAngle-armMotor.getCount()), power, true);
	return (0);
}

