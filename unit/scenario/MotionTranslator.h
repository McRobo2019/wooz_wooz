#ifndef MOTION_TRANSLATOR_
#define MOTION_TRANSLATOR_

#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cstring>
#include "Scenario.h"
#include "common.h"
#include "Logger.h"

using namespace std;

class Pos {
	public:
		int x;
		int y;
		Pos(int _x, int _y):x(_x),y(_y){}
		void set(int _x, int _y) {
			x = _x;
			y = _y;
		}
};

class ConveyPath {
	public:
		vector <Pos> enter;         //0
		vector <Pos> hold_block[4]; //1,3,5,7
		vector <Pos> conv_block[4]; //2,4,6,8
		vector <Pos> exit;          //9

		void debug_print() {

			print_new_line("## enter");
			for (unsigned int i = 0; i < enter.size(); i++) {
				print_new_line("x = %d, y = %d", enter[i].x, enter[i].y);
			}

			for (unsigned int i = 0; i < 4; i++) {
				print_new_line("## hold block%d", i+1);
				for (unsigned int j = 0; j < hold_block[i].size(); j++) {
					print_new_line("x = %d, y = %d", hold_block[i][j].x, hold_block[i][j].y);
				}

				print_new_line("## conv block%d", i+1);
				for (unsigned int j = 0; j < conv_block[i].size(); j++) {
					print_new_line("x = %d, y = %d", conv_block[i][j].x, conv_block[i][j].y);
				}
			}

			print_new_line("## exit");
			for (unsigned int i = 0; i < exit.size(); i++) {
				print_new_line("x = %d, y = %d", exit[i].x, exit[i].y);
			}
		}

};

class MotionTranslator {

	public:
		MotionTranslator();
		void set_scenario(vector<Scenario> *scn);
		int read_path_file();
		void debug_print();
		void terminal_print();
		int translate(int type, vector<Pos> &path);
		int insert_scenario(int pre_sno);

	private:
		Pos *dir;
		ConveyPath convpath;
		vector<Scenario> *course_scenario;
		vector<Scenario> block_scenario;
		vector<Scenario> forward;
		vector<Scenario> turn_right;
		vector<Scenario> turn_left;
		vector<Scenario> hold;
		vector<Scenario> release;

		// コマンド定義
		void set_motion();
};
                              
#endif //MOTION_TRANSLATOR_
