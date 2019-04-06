#ifndef LINE_MONITOR_H_
#define LINE_MONITOR_H_

#include "ev3api.h"
#include "ColorSensor.h"
#include "TouchSensor.h"

using namespace ev3api;

class LineMonitor {

	public:
		explicit LineMonitor(ColorSensor& color, TouchSensor& touch);
		int calibrate();
		int getPosition();
		int getColor();
		float getTarget();
		void getRGB(rgb_raw_t *rgb);


	private:
		ColorSensor &color_sensor;
		TouchSensor &touch_sensor;
		int white;
		int black;
		float target;
		rgb_raw_t rgb_val;
	
};

#endif//LINE_MONITOR_H_
