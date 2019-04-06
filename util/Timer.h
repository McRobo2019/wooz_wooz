#ifndef TIMER_H_
#define TIMER_H_

#include "Clock.h"

using namespace ev3api;

#define timer_reset Timer::reset
#define timer_now Timer::now

class Timer {

	typedef Clock base;

 	public:
 		Timer();
		static void reset();
		static int now();

	private:
		static Clock *clock;
};

#endif // TIMER_H_
