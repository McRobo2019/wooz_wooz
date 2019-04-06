#include "Navigator.h"
#include "Logger.h"

Navigator::Navigator(Motor &left, Motor &right){
	pos_estimator = new PosEstimator(left, right);
	map = new Map();
	milage = 0;
}

int Navigator::compute(int map_no) {

	pos_estimator->compute();
	milage = pos_estimator->getMilage();
	print_new_line("milage = %f", milage);
	return(map->is_in_section(map_no, milage));
}

int Navigator::resetMilage(int type) {
	if ( type == 0) {
		// typeが0ならばmilage=0とする
		pos_estimator->resetMilage(0);
	} else {
		// typeが0でなければ実距離と目標距離の差をmilageの初期値とする
		pos_estimator->resetMilage(map->getSurplus());
	}
	return(0);
}

int Navigator::setMilage(float milage) {
	pos_estimator->setMilage(milage);
	return(0);
}
