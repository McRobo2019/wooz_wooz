#include "MotionSequencer.h"

MotionSequencer::MotionSequencer(Motor& left, Motor &right, Motor &arm, ColorSensor &colorsens){
	swatch  = new StopWatch();
	volante = new Volante(left, right);
	fwrd    = new FowardTravel(volante, swatch);
	turn    = new Turn(volante, swatch);
	pivt    = new PvtTurn(volante, swatch);
	spin    = new SpinTurn(volante, swatch);
	col_ide = new ColorIdentify(volante,swatch,arm,colorsens);
	EXP_check = new EXPchecker(volante, swatch);
	stop_motion = new Stop(volante, swatch);
	cross_check = new Crosschecker(volante,swatch,colorsens);
	motionseq = fwrd;
}

int MotionSequencer::drive() {
/*	int status = 0;
	for (unsigned int i = 0; i < motion_list.size(); i++ ) {
		if ((status = motion_list[i]->drive()) != 0) {
			// コマンドが失敗
			return status;
		}
	}
*/	
	motionseq->drive();
	return 0;
}

int MotionSequencer::stop() {
	return volante->stop();
}

int MotionSequencer::set_move_mode(int mode, int type) {
	
	if ( mode == DRIVE_MODE_MTN::MOTION_FWD_TRVL ) {
		print_new_line("set_fwrd");
		motionseq = fwrd;
//		motion_list.push_back(fwrd);
	} else if ( mode == DRIVE_MODE_MTN::MOTION_NML_TURN ) {
		print_new_line("set_turn");
		motionseq = turn;
//		motion_list.push_back(turn);
	} else if ( mode == DRIVE_MODE_MTN::MOTION_SPN_TURN ) {
		print_new_line("set_spin");
		motionseq = spin;
//		motion_list.push_back(spin);
	} else if ( mode == DRIVE_MODE_MTN::MOTION_PVT_TURN ) {
		print_new_line("set_pivt");
		motionseq = pivt;
//		motion_list.push_back(turn);
	} else if ( mode == DRIVE_MODE_MTN::MOTION_COL_CHCK ) {
		print_new_line("set_col_check");
		motionseq = col_ide;
	} else if ( mode == DRIVE_MODE_MTN::MOTION_EXP_CHCK ) {
		print_new_line("set_EXP_check");
		motionseq = EXP_check;
//		motion_list.push_back(turn);
	} else if ( mode == DRIVE_MODE_MTN::MOTION_STOP ) {
		print_new_line("set_STOP");
		motionseq = stop_motion;
//		motion_list.push_back(turn);
	} else if ( mode == DRIVE_MODE_MTN::MOTION_CRS_CHCK ) {
		print_new_line("set_Cross_check");
		motionseq = cross_check;
	} else {
		print_new_linef("ERROR: MotionSequencer: undefined motion mode %d", mode);
	}
	motionseq->set_type(type);
	print_new_line("MotionSequencer: set mode = %d, type = %d", mode, type);

	return 0;
}

float MotionSequencer::getLastPower() {
	return( volante->getPower() );
}

int MotionSequencer::setIniPower(float power_ini) {
	volante->setPower(power_ini);
	return(0);
}
