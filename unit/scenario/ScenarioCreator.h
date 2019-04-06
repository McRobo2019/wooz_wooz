#ifndef SCENARIO_CREATOR_H_
#define SCENARIO_CREATOR_H_

#include "Scenario.h"
#include "MotionTranslator.h"
#include <vector>

using namespace std;

class ScenarioCreator {

	public:
		Scenario temp;
		explicit ScenarioCreator(vector<Scenario> *scn);
		int update_scenario(int scn_no, int condition);
		int insert_block_scenario();

	private:
		vector<Scenario> *scenario;
		MotionTranslator motiontrans;
};

#endif //SCENARIO_CREATOR_H_
