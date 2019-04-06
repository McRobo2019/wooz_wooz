#include "TouchStarter.h"
#include "SoundPlayer.h"

TouchStarter::TouchStarter(TouchSensor& touchsens):touchsensor(touchsens) {
	;
}

int TouchStarter::isPressed() {
	return touchsensor.isPressed();
}

int TouchStarter::on() {
	if ( status == 0 ) {
		if ( isPressed() ) {
			status = 1;
		}
	}
	return status;
}

int TouchStarter::off() {
	if ( status == 1 ) {
		if ( isPressed() ) {
			status = 0;
		}
	}
	return status;
}

int TouchStarter::waitPressed() {
	while (!isPressed());
	while (isPressed());
	sound_play_chime1(NOTE_C5, 30);
	tslp_tsk(500);

	return 1;
}		

