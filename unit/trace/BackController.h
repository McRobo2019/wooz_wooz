
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


			//-----StraightControll.h����̕ύX�_1-------//


			pos_err = target - curpos;


			Power_R = volante->getPower_r();
			Power_L = volante->getPower_l();

			power_err =  (Power_L+0.001) / (Power_R+0.001) -1 ; //+0.001�̓[�����h�~
			integral += power_err;

			ajust = Kp_back * power_err + Ki_back * integral;


   // PID�����p�@�ϑ��f�[�^���O�p�@��������
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
   // PID�����p�@�ϑ��f�[�^���O�p�@�����܂�
		
*/


			volante->steerPID(pos_err - ajust);
			//-----------�ύX�_1�I���------------------//

			volante->calc_speed_pid();
			volante->drive();

			return 0;
		}
//-----StraightControll.h����̕ύX�_2-------//
	private:
		vector<float> Kp_back_tmp; // ��ޗpKp�l���p�ϐ�
		vector<float> Ki_back_tmp; // ��ޗpKi�l���p�ϐ�
		float Kp_back   = 0;	   // ��ޗpKp�i�[�p�ϐ�
		float Ki_back   = 0;	   // ��ޗpKi�i�[�p�ϐ�
		float power_err = 0;	   // ���E�ւ�PWM�̍�
		float Power_R   = 0;	   // �E�ւ�PWM(=Power)
		float Power_L   = 0;	   // ���ւ�PWM(=Power)
		float ajust     = 0;	   // ��ޗp�̒����l
		float pos_err   = 0;	   // �J���[�Z���T�̖ڕW�l�Ƃ̍�
		
//---------�ύX�_2�I���---------------------//
};

#endif // BACK_CONTROLLER_H_


