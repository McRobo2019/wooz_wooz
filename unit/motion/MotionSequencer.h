#ifndef MOTION_SEQUENCER_H_
#define MOTION_SEQUENCER_H_

#include "common.h"
#include "ColorSensor.h"
#include "Volante.h"
#include "Motion.h"
#include "Command.h"
#include "FowardTravel.h"
#include "Turn.h"
#include "SpinTurn.h"
#include "PvtTurn.h"
#include "EXPchecker.h"
#include "ColorIdentify.h"
#include "Stop.h"
#include "CrossChecker.h"

#include <vector>

using namespace std;

class MotionSequencer {

	public:
		explicit MotionSequencer(Motor& left, Motor &right, Motor &arm, ColorSensor &colorsens);
		int drive();
		int stop();
		int set_move_mode(int mode, int type);
		float getLastPower();
		int setIniPower(float power_ini);

	private:
		Volante       *volante;
		FowardTravel  *fwrd;
		Turn          *turn;
		SpinTurn      *spin;
		PvtTurn       *pivt;
		EXPchecker    *EXP_check;
		ColorIdentify *col_ide;
		Motion        *motionseq;
		StopWatch     *swatch;
		Stop          *stop_motion;
		Crosschecker  *cross_check;
		vector<Motion*> motion_list;
};

#endif // MOTION_SEQUENCER_H_
