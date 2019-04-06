#include "StopWatch.h"

StopWatch::StopWatch():start_time(0), stop_time(0), cycle(0) { clock = new Clock(); }
void StopWatch::start() { start_time = clock->now(); }
void StopWatch::stop()  { stop_time  = 0; };
void StopWatch::set_cycle(int cyc) { cycle  = cyc; }
unsigned int StopWatch::now() { return ((clock->now()) - start_time); };

unsigned int StopWatch::get_cycle_time() { 
	if ( cycle != 0 ) {
		return ( ((clock->now()) - start_time) % cycle );
	} else {
		return 0;
	}
}

float StopWatch::get_cycle_per() { 
	if ( cycle != 0 ) {
		return ( (float)((clock->now() - start_time) % cycle) / (float)cycle * 100);
	} else {
		return 0;
	}
}