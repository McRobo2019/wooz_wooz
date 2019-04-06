#ifndef EXCHECKER_H_
#define EXCHECKER_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "common.h"
#include "Motion.h"
#include "Logger.h"
#include "ev3api.h"
#include "SonarSensor.h"

using namespace std;
using namespace ev3api;

// 新幹線検出用パラメータ
#define FAR_THRESH (30)    // 新幹線が手前に見えるときの最大距離
#define NEAR_THRESH (10)   // 新幹線が手前に見えるときの最大距離
#define ALARM_TIME (1000)  //[ms] 新幹線が通過後の安全確保マージン
#define REJECT_TIME (20000) // [ms] 強制脱出時間
#define SENS_WAIT_TIME (5) // ソナー計測のためのウェイト時間
#define IN_THRESH1 (20)
#define IN_THRESH2 (40)
#define OUT_THRESH1 (78)
#define OUT_THRESH2 (98)


class EXPchecker: public Motion {

	public:
		EXPchecker(Volante *vlnt, StopWatch *watch):Motion(vlnt, watch) {

			FILE *fp;

			if (( fp = fopen("/ev3rt/def/exp_check.def", "r")) == NULL) {
				printf("can't read file: exp_check.def\n");
			} else {
				char line[MAX_STR] = {0};
				while (fgets(line, sizeof(line), fp)) {
					if ( line[0] == '#'  ) {
						continue;
					} else if ( line[0] == '\n' ) {
						continue;
					} else {
						;
					}
					char *token = strtok( line, ",");
					select_type.push_back( atoi(token) );
					
				}

			}

			for (unsigned int i = 0; i < select_type.size(); i++) {
				print_new_linef("%d, type = %d\n", select_type[i]);
			}

			fclose(fp);
			
			ev3_sensor_config(EV3_PORT_2,ULTRASONIC_SENSOR);

		}
		virtual int drive() {

			print_new_line("EXP_check_start");
			
			ulong_t IN_time;
			ulong_t time;
			ulong_t T1;
			ulong_t T2;
			ulong_t T3;
			ulong_t rot_T;
			float cyc_per = 0;
			int distance = 0;
			int i = 0;
			int counter = 0;
			int selecter = select_type[type];

			if(selecter == 0){ // 周期演算のための観測モード

				// 関数呼び出された時の時間を保存
				get_tim(&IN_time);
				
				// 新幹線が目の前に来るまで待機
				while( counter < 3 )
				{
					get_tim(&time);
					counter = 0;
					// 測距センサの値取得
					for(i = 0;i<4;i++)
					{
						distance = ev3_ultrasonic_sensor_get_distance(EV3_PORT_2);
						tslp_tsk(SENS_WAIT_TIME); 
						print_new_line("dist1 = %d", distance);
						if(distance > NEAR_THRESH && distance < FAR_THRESH){ counter++; }
					}

					// もし指定時間以内に来なければ強制脱出
					if( (time - IN_time) > REJECT_TIME )
					{
						print_new_line("EXP_check_escape");
						return 0;
					}
				}
				get_tim(&T1);
				ev3_speaker_play_tone(NOTE_F4,100);
				tslp_tsk(1000); 
				
				counter = 0;
				// 新幹線が目の前に来るまで待機
				while( counter < 3 )
				{
					get_tim(&time);
					counter = 0;
					// 測距センサの値取得
					for(i = 0;i<4;i++)
					{
						distance = ev3_ultrasonic_sensor_get_distance(EV3_PORT_2);
						tslp_tsk(SENS_WAIT_TIME); 
						print_new_line("dist2 = %d", distance);
						if(distance > NEAR_THRESH && distance < FAR_THRESH){ counter++; }
					}

				}
				get_tim(&T2);
				ev3_speaker_play_tone(NOTE_F4,100);
				tslp_tsk(1000); 
				
				counter = 0;
				// 新幹線が目の前に来るまで待機
				while( counter < 3 )
				{
					get_tim(&time);
					counter = 0;
					// 測距センサの値取得
					for(i = 0;i<4;i++)
					{
						distance = ev3_ultrasonic_sensor_get_distance(EV3_PORT_2);
						tslp_tsk(SENS_WAIT_TIME); 
						print_new_line("dist3= %d", distance);
						if(distance > NEAR_THRESH && distance < FAR_THRESH){ counter++; }
					}

				}
				get_tim(&T3);
				ev3_speaker_play_tone(NOTE_F4,100);
				
				rot_T = ( (T2-T1) + (T3-T2) )/2;
				print_new_line("rot_T1 = %d, rot_T2 = %d, rot_T = %d", (T2-T1),(T3-T2),rot_T);

				swatch->start();
				swatch->set_cycle(rot_T);

				print_new_line("EXP_check_end");
				return 0;
				
			} else if( selecter == 1){ // 入口周辺安全確認
				while(1){
					cyc_per = swatch->get_cycle_per();
					print_new_line("per = %f",cyc_per);
					tslp_tsk(50); 
					
					//if( IN_THRESH1 < cyc_per && cyc_per < IN_THRESH2 ){break;}
					if( IN_THRESH1 < cyc_per && cyc_per < IN_THRESH2 ){break;}
					
				}
				print_new_line("EXP_IN_check_end");
				ev3_speaker_play_tone(NOTE_F4,100);
				return 0;
				
			} else if( selecter == 2){ // 出口周辺安全確認
				while(1){
					cyc_per = swatch->get_cycle_per();
					print_new_line("per = %f",cyc_per);
					tslp_tsk(50); 
					if( OUT_THRESH1 < cyc_per && cyc_per < OUT_THRESH2 ){break;}
				}
				print_new_line("EXP_OUT_check_end");
				ev3_speaker_play_tone(NOTE_F4,100);
				return 0;
				
			} else{
				ev3_speaker_play_tone(NOTE_F4,500);
				print_new_line("EXP_check_error. Please check the select_type.");
				return 0;
			}
		}
	
	private:
		vector<int> select_type;
		//sensor_port_t sonar_sensor = EV3_PORT_2;
};

#endif // EXCHECKER_H_
