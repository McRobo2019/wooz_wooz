
#ifndef TOUCH_STARTER_H_
#define TOUCH_STARTER_H_

#include "Switcher.h"
#include "TouchSensor.h"

using namespace ev3api;

class TouchStarter : public Switcher {

	public:
		explicit TouchStarter(TouchSensor &touchsens);
		virtual int isPressed();
		virtual int on();
		virtual int off();
		virtual int waitPressed();

	private:
		TouchSensor& touchsensor;
};

#endif // TOUCH_STARTER_H_
