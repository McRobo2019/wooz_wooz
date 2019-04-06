#include "TraceController.h"
#include "SoundPlayer.h"
#include "Logger.h"


TraceController::TraceController(LineMonitor *linemon, Volante *vlnt):
	linemonitor(linemon), volante(vlnt){
		; //nop
}

int TraceController::calibrate() {
	line_target = linemonitor->calibrate();
	tslp_tsk(1000);
	return 0;
}

int TraceController::stop() {
	for (int i = 0; i < 100; i++) {
		volante->stop();
		volante->reset_power();
	}
	sound_play_chime2(NOTE_C4, 100);
	return 0;
}

