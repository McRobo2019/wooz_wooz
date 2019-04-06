#ifndef BATTERY_MONITOR_H_
#define BATTERY_MONITOR_H_

#include "ev3api.h"

class BatteryMonitor {
	public:
		explicit BatteryMonitor();

		void measure();

		float get_voltage();
	
	private:
		float voltage_mv;
};

#endif  //BATTERY_MONITOR_H_
