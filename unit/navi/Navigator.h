#ifndef NAVIGATOR_H_
#define NAVIGATOR_H_

#include "PosEstimator.h"
#include "Map.h"

class Navigator {

	public:
		explicit Navigator(Motor &left, Motor &right);
		int compute(int map_no);
		int resetMilage(int type);
		int setMilage(float milage);

	private:
		PosEstimator *pos_estimator;
		Map *map;
		float milage;
};

#endif //NAVIGATOR_H_
