#ifndef STOP_WATCH_H_
#define STOP_WATCH_H_

#include "Clock.h"

using namespace ev3api;

class StopWatch {

	typedef Clock base;

 	public:
 		StopWatch();
		void start();
		void stop();
		void set_cycle(int cyc);
		unsigned int now();
		unsigned int get_cycle_time();
		float get_cycle_per();

	private:
		Clock *clock;
		unsigned int start_time;
		unsigned int stop_time;
		unsigned int cycle;
};

#endif // STOP_WATCH_H_
