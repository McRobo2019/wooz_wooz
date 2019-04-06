
#ifndef BACK_CONTROLLER_H_
#define BACK_CONTROLLER_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "TraceController.h"

class BackController : public TraceController {

	public:
		BackController(LineMonitor *linemon, Volante *vlnt):
			TraceController(linemon, vlnt){

				FILE *fp;

				if (( fp = fopen("/ev3rt/def/bck_pidv.def", "r")) == NULL) {
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
									case 1: mode.pid.Kp  = atof(token); break;
									case 2: mode.pid.Ki  = atof(token); break;
									case 3: mode.pid.Kd  = atof(token); break;
									case 4: Kp_back_tmp.push_back( atof(token) ); break;
									case 5: Ki_back_tmp.push_back( atof(token) ); break;
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
			Kp_back = Kp_back_tmp[type];
			Ki_back = Ki_back_tmp[type];
			
			return 0;
		}

		virtual int drive() {

			static float integral = 0;
			static float target = linemonitor->getTarget();
			curpos = linemonitor->getPosition();


			//-----StraightControll.hからの変更点1-------//


			pos_err = target - curpos;


			Power_R = volante->getPower_r();
			Power_L = volante->getPower_l();

			power_err =  (Power_L+0.001) / (Power_R+0.001) -1 ; //+0.001はゼロ割防止
			integral += power_err;

			ajust = Kp_back * power_err + Ki_back * integral;


   // PID調整用　観測データログ用　ここから
/*
			print_new_line("%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
					target,   //1
					curpos,   //2
					pos_err,  //3
					Power_R,  //4
					Power_L,  //5
					power_err, //6
					integral,  //7
					Kp_back * power_err, //8
					Ki_back * integral,  //9
					ajust,   //10
				    Kp_back, //11
					Ki_back, //12
					pos_err-ajust //13
												);
   // PID調整用　観測データログ用　ここまで
		
*/


			volante->steerPID(pos_err - ajust);
			//-----------変更点1終わり------------------//

			volante->calc_speed_pid();
			volante->drive();

			return 0;
		}
//-----StraightControll.hからの変更点2-------//
	private:
		vector<float> Kp_back_tmp; // 後退用Kp獲得用変数
		vector<float> Ki_back_tmp; // 後退用Ki獲得用変数
		float Kp_back   = 0;	   // 後退用Kp格納用変数
		float Ki_back   = 0;	   // 後退用Ki格納用変数
		float power_err = 0;	   // 左右輪のPWMの差
		float Power_R   = 0;	   // 右輪のPWM(=Power)
		float Power_L   = 0;	   // 左輪のPWM(=Power)
		float ajust     = 0;	   // 後退用の調整値
		float pos_err   = 0;	   // カラーセンサの目標値との差
		
//---------変更点2終わり---------------------//
};

#endif // BACK_CONTROLLER_H_


