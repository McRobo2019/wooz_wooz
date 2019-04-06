
#ifndef EDGE_CHANGER_H_
#define EDGE_CHANGER_H_

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "TraceController.h"
#include "Logger.h"

class EdgeChanger : public TraceController {

	public:

		EdgeChanger(LineMonitor *linemon, Volante *vlnt):
			TraceController(linemon, vlnt){

			FILE *fp;

			if ( (fp = fopen("/ev3rt/def/linetrace.def", "r")) == NULL ) {

				print_new_linef("ERROR: can't read line.def");

			} else {
				char line[MAX_STR]={0};
				char  *key = NULL;
				char  *num  = NULL;
				float val = 0;

				while(fgets(line, sizeof(line), fp)) {
					if ( line[0] == '#' ) continue; //先頭が#はコメント扱い
					key  = strtok(line, "=");
					num  = strtok(NULL, "=");
					val = atof(num);
					if (!strcmp(key, "edge_steer_angle")) {
						edge_steer_angle = val;
						print_new_linef("linetrace.def: edge_steer_angle = %f", edge_steer_angle);
					} else if (!strcmp(key, "edge_velocity")) {
						edge_velocity = val;
						print_new_linef("linetrace.def: edge_velocity = %f", edge_velocity);
					} else if (!strcmp(key, "edge_repeat_count")) {
						edge_repeat_count = val;
						print_new_linef("linetrace.def: edge_repeat_count = %d", edge_repeat_count);
					} else {
						print_new_linef("ERROR: linetrace.def = undefined key word = %s", key);
					}
				}

				fclose(fp);
			}
			
		};

		virtual int set_type(int type) {
			volante->set_speed(edge_velocity);
			//type = 1 : R2L, type = 0; L2R
			flag_R2L = (type ? 1 : 0);
			return 0;
		}

		virtual int drive() {

			if (flag_R2L) {
				//　右エッジから左エッジへの切り替え
				volante->steer(-edge_steer_angle);
				volante->reverse_steer(true);

			} else {
				//　左エッジから右エッジへの切り替え
				volante->steer(edge_steer_angle);
				volante->reverse_steer(false);
			}
			
 
//			for (int i = 0; i < edge_repeat_count; i++) {
				volante->calc_speed_pid();
				volante->drive();
//			}
			return 0;
		}
	private:
		int flag_R2L;
};

#endif // EDGE_CHANGER_H_
