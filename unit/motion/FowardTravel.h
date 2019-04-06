#ifndef FOWARD_TRAVEL_H_
#define FOWARD_TRAVEL_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "common.h"
#include "Motion.h"
#include "Logger.h"
#include "Timer.h"

class FowardTravel : public Motion {

	public:
		// 距離とvelは引数がない場合は以下となる
		FowardTravel(Volante *vlnt, StopWatch *watch): Motion(vlnt, watch) {

			FILE *fp;

			if (( fp = fopen("/ev3rt/def/fw_travel.def", "r")) == NULL) {
				printf("can't read file: fw_travel.def\n");
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
					type_vel.push_back( atof(token) );
					int count = 1;
					while ( token != NULL ) {
						token = strtok( NULL, "," );
						if ( token != NULL ) {
							switch (count++) {
								case 1: type_dist.push_back( atof(token) ); break;
								case 2: type_adjuster.push_back( atof(token) ); break;
								default : break;
							}
						}
					}
				}
			}
			for (unsigned int i = 0; i < type_dist.size(); i++) {
				print_new_linef("%d, distance = %f[mm], vel = %f\n", type_dist[i], type_vel[i]);
			}
			fclose(fp);
		}


		// driveをコールする前にset_typeする必要がある
		virtual int drive() {
			print_new_line("Forward_drive_start");
			
			int   pre_rot_R = volante->get_angle_wheel_r();  // １サイクル前の右モーターの回転角
			int   pre_rot_L = volante->get_angle_wheel_l();  // １サイクル前の左モーターの回転角
			int   cur_rot_R = 0;	// 今回の右モーターの回転角
			int   cur_rot_L = 0;	// 今回の左モーターの回転角
			float dif_rot_L = 0;	// 1時刻前からの右輪の回転角の差分
			float dif_rot_R = 0;	// 1時刻前からの左輪の回転角の差分
			float dist_R = 0.0;		// スタートしてからの進行距離 [mm]
			float dist_L = 0.0;		// スタートしてからの進行距離 [mm]
			float duty_R = 0;		// 右モータPWM値
			float duty_L = 0;		// 左モータPWM値
			float gain_R = 1;		// 左右輪差補正用ゲイン
			float gain_L = 1;		// 左右輪差補正用ゲイン
			
			int distance_mm = type_dist[type];	// 目標移動距離
			float duty = 0.0;
//			volante->reset_power();
			
			volante->set_speed(type_vel[type]);
//			print_new_line("pre_rot_R = %d, L = %d", pre_rot_R, pre_rot_L);
			
			while( distance_mm * distance_mm > (dist_R * dist_R + dist_L * dist_L) / 2 ) {
				
				// 速度制御追加 0903 tamamru
				duty = volante->calc_speed_pid();
//				print_new_line("duty = %f", duty);
				
				// 現在のモーターの回転角を取得
				cur_rot_R = volante->get_angle_wheel_r();
				cur_rot_L = volante->get_angle_wheel_l();
				
				// 前回値との差分を計算
				dif_rot_R = (float)cur_rot_R - (float)pre_rot_R;
				dif_rot_L = (float)cur_rot_L - (float)pre_rot_L;
//				print_new_line("dif_rot_R = %f, L = %f", dif_rot_R, dif_rot_L);
				// 開始からの左右輪の駆動距離[mm]
				dist_R += (WHEEL_RADIUS*PI)*(dif_rot_R/180);
				dist_L += (WHEEL_RADIUS*PI)*(dif_rot_L/180);
				print_new_line("dist_R = %f, L = %f", dist_R, dist_L);
				// 現在値を前回値として保存
				pre_rot_R = cur_rot_R;
				pre_rot_L = cur_rot_L;

				// adjuster　1:左右回転差補正ON 0:OFF
				if(type_dist[type] == 1){
					// 左右回転差補正
					if( dif_rot_L * dif_rot_L  > dif_rot_R * dif_rot_R){
						if( dif_rot_L < 5 ){ gain_L = 1;}
						else{ gain_L = 1 - (dif_rot_L - dif_rot_R) / dif_rot_L;}
						gain_R = 1; 
					} else { // dif_rot_L * dif_rot_L <= dif_rot_R * dif_rot_R
						if( dif_rot_R < 5  ){ gain_R = 1;}
						else{ gain_R = 1 - (dif_rot_R - dif_rot_L) / dif_rot_R;}
						gain_L = 1;
					}
				} else{
					gain_L = 1;
					gain_R = 1;
				}
//				print_new_line("gain_R = %f, L = %f", gain_R, gain_L);
				
				duty_L = duty * gain_L;
				duty_R = duty * gain_R;
//				print_new_line("duty_R = %f, L = %f", duty_R, duty_L);

				volante->setPWM(duty_L, duty_R);

				tslp_tsk(30);
			}
			
			// 終了処理
//			volante->stop();
//			ev3_speaker_play_tone(NOTE_D4,100);

			print_new_line("Forward_drive_end");
			return 0;
		}

	private:
		vector <int> type_dist;
		vector <float> type_vel;
		vector <float> type_adjuster;
		

};

#endif // FOWARD_TRAVEL_H_
