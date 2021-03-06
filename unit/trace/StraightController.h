
#ifndef STRAIGHT_CONTROLLER_H_
#define STRAIGHT_CONTROLLER_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "TraceController.h"
#include "Timer.h"

class StraightController : public TraceController {

	public:
		StraightController(LineMonitor *linemon, Volante *vlnt):
			TraceController(linemon, vlnt){

				FILE *fp;

				if (( fp = fopen("/ev3rt/def/str_pidv.def", "r")) == NULL) {
					printf("can't read file\n");
				} else {
					char line[MAX_STR] = {0};
					while (fgets(line, sizeof(line), fp)) {
						PidMode mode;
						if ( line[0] == '#'  ) {
							continue;
						} else if ( line[0] == '\n' ) {
							continue;
						} else {
							;
						}
						char *token = strtok( line, ",");
						mode.v = atof(token);
						int count = 1;
						while ( token != NULL ) {
							token = strtok( NULL, "," );
							if ( token != NULL ) {
								switch (count++) {
									case 1: mode.pid.Kp = atof(token); break;
									case 2: mode.pid.Ki = atof(token); break;
									case 3: mode.pid.Kd = atof(token); break;
									default : break;
								}
							}
						}
						control_param.push_back(mode);
					}

				}

				for (unsigned int i = 0; i < control_param.size(); i++) {
					print_new_linef("v  = %f\n", control_param[i].v);
					print_new_linef("Kp = %f\n", control_param[i].pid.Kp);
					print_new_linef("Ki = %f\n", control_param[i].pid.Ki);
					print_new_linef("Kd = %f\n", control_param[i].pid.Kd);
				}

			}
		virtual int  set_type(int type) {
			volante->set_pid(control_param[type].pid);
			volante->set_speed(control_param[type].v);
			return 0;
		}

		virtual int drive() {

			print_new_line("%d", timer_now());
			static float target = linemonitor->getTarget();
			print_new_line("%d", timer_now());
			curpos = linemonitor->getPosition();
			print_new_line("%d", timer_now());
			volante->steerPID(target - curpos);
			print_new_line("%d", timer_now());
			volante->calc_speed_pid();
			print_new_line("%d", timer_now());
			volante->drive();
			print_new_line("%d", timer_now());

			return 0;
		}
};

#endif // STRAIGHT_CONTROLLER_H_


