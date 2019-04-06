#ifndef DRIVER_H_
#define DRIVER_H_

#include <vector>
#include "Navigator.h"
#include "Scenario.h"
#include "ScenarioCreator.h"
#include "LineTracer.h"
#include "MotionSequencer.h"

using namespace std;


class Driver {

	public:
	 	explicit Driver(Motor &left, Motor &right, LineTracer *ltracer, MotionSequencer *mseqencer);
		int drive();
		int goNextScenario();
		int getChangeState();

	private:
		Navigator *navi;
		LineTracer *linetracer;
		MotionSequencer *motionseq;
		ScenarioCreator *scncreator;
		vector<Scenario> scenario;
		int cur_scenario;
		int pre_scenario;
		float pre_power;
		int change_state;
};

#endif //DRIVER_H_
