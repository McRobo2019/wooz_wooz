#include "BatteryMonitor.h"
#include "Logger.h"

BatteryMonitor::BatteryMonitor(){
	voltage_mv = 0;
}

void BatteryMonitor::measure() {
	voltage_mv = ev3_battery_voltage_mV();
//	print_new_line("battery voltage = %f[V]", (voltage_mv / 1000));
}

float BatteryMonitor::get_voltage() {
	return voltage_mv/1000;
}
