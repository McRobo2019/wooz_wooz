#ifndef SCENARIO_H_
#define SCENARIO_H_

class Scenario {
	public:
		Scenario(int _no=0, int _map=0, int _mode=0, int _type=0):
			no(_no),map(_map),mode(_mode),type(_type){;}
		void set(int _no, int _map, int _mode, int _type){
			no   = _no;
			map  = _map;
			mode = _mode;
			type = _type;
		}
		int no;
		int map;
		int mode;
		int type;
};

#endif //SCENARIO_H_
