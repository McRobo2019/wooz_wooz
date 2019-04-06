#include "Timer.h"

using namespace ev3api;

Clock *Timer::clock;

Timer::Timer() { clock = new Clock(); };
void Timer::reset() { clock->reset(); };
int Timer::now() { return clock->now(); };
