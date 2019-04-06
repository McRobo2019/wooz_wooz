#ifndef COL_IDENTIFY_H_
#define COL_IDENTIFY_H_

#include "ev3api.h"
#include "Motion.h"
#include "Motor.h"
#include "ColorSensor.h"

using namespace ev3api;

class ColorIdentify : public Motion {

	public:
		explicit ColorIdentify(Volante *vlnt, StopWatch *watch, Motor& arm, ColorSensor& sensor);
		virtual int drive();
		int getColor();
	
	private:
		Motor& armMotor;
		ColorSensor& colorSensor;
		int getColorNum();
		int getBlueVal();
		int getAngle();
		int setArmAngle(int tgtAngle);
		colorid_t color;
		int color_num;
		int palette[3];
		int power;
		int blue_val;
		rgb_raw_t rgb_val;
};

#endif //COL_IDENTIFY_H_
