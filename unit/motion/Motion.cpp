#include "Motion.h"
#include "SoundPlayer.h"

Motion::Motion(Volante *vlnt, StopWatch *watch):volante(vlnt), swatch(watch) {
	; //nop
}

int Motion::set_type(int type) {
	return this->type = type;
}

int Motion::stop() {
	volante->stop();
	sound_play_chime2(NOTE_C4, 100);
	return 0;
}
