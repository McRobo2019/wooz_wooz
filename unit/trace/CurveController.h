
#ifndef CURVE_CONTROLLER_H_
#define CURVE_CONTROLLER_H_

#include <cstdio>
#include <cstdlib>
#include "TraceController.h"

class CurveController : public TraceController {

	public:
		CurveController(LineMonitor *linemon, Volante *vlnt):
			TraceController(linemon, vlnt){

			FILE *fp;

			if (( fp = fopen("/ev3rt/def/crv_pidv.def", "r")) == NULL) {
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
			
		};

		virtual int set_type(int type) {
			volante->set_pid(control_param[type].pid);
			volante->set_speed(control_param[type].v);
			return 0;
		}

		virtual int drive() {

			static float target = linemonitor->getTarget();
			curpos = linemonitor->getPosition();
			volante->steerPID(target - curpos);
			volante->calc_speed_pid();
			volante->drive();

			return 0;
		}
};

#endif // STRAIGHTE_CONTROLLER_H_
