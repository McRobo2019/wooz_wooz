#ifndef MAP_H_
#define MAP_H_

#include "PosEstimator.h"
#include <unordered_map>

using namespace std;

class Map {

	public:
		explicit Map();
		int is_in_section(int map_no, float milage);
		float getSurplus();

	private:
		unordered_map<int, float> point;
		float surplus;
};

#endif //MAP_H_
