#ifndef SPIN_TURN_H_
#define SPIN_TURN_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "common.h"
#include "Motion.h"
#include "Logger.h"

class SpinTurn: public Motion {

	// 旧senkai その場で回る
	// 対して方輪だけ回すのはpivot turnと名付けます

	public:
		SpinTurn(Volante *vlnt, StopWatch *watch):Motion(vlnt, watch) {
			
			FILE *fp;
			
			if (( fp = fopen("/ev3rt/def/spin_turn.def", "r")) == NULL) {
				printf("can't read file: spin_turn.def\n");
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
					type_deg.push_back( atoi(token) );
					int count = 1;
					while ( token != NULL ) {
						token = strtok( NULL, "," );
						if ( token != NULL ) {
							switch (count++) {
								case 1: type_power.push_back( atoi(token) ); break;
								default : break;
							}
						}
					}
				}
			}
			for (unsigned int i = 0; i < type_deg.size(); i++) {
				print_new_linef("%d, deg = %d[mm], power = %d\n", type_deg[i], type_power[i]);
			}
			fclose(fp);
		}


		virtual int drive() {
			
			print_new_line("Spin_turn_start");
			
			int distance_mm  = 0;   // 指定角度から算出した片輪が進む円弧の長さ
			int pre_rot_R = volante->get_angle_wheel_r();  // １サイクル前の右モーターの回転角
			int pre_rot_L = volante->get_angle_wheel_l();  // １サイクル前の左モーターの回転角
			int cur_rot_R = 0;  // 今回の右モーターの回転角
			int cur_rot_L = 0;  // 今回の左モーターの回転角
			float dif_rot_L = 0;  // 1時刻前からの右輪の回転角の差分
			float dif_rot_R = 0;  // 1時刻前からの左輪の回転角の差分
			float abs_dif_rot_L = 0;  // 1時刻前からの右輪の回転角の差分
			float abs_dif_rot_R = 0;  // 1時刻前からの左輪の回転角の差分			
			float dist_R = 0;   // スタートしてからの進行距離[mm]
			float dist_L = 0;   // スタートしてからの進行距離 [mm]
			float duty_R = 0;
			float duty_L = 0;
			float gain_R = 1;
			float gain_L = 1;
			
			int deg   = type_deg[type];
			int power = type_power[type];
			
			// TODO:他の関数と共通の表現にする
			// できればキャンセルされる計算は除く
			distance_mm = (TRED_WIDTH * PI* deg) / 360 * 1.03;
			
			while( distance_mm * distance_mm > (dist_R * dist_R + dist_L * dist_L) / 2 ) {
				// 現在のモーターの回転角を取得
				cur_rot_R = volante->get_angle_wheel_r();
				cur_rot_L = volante->get_angle_wheel_l();
//				print_new_line("cur_rot_R = %d, L = %d", cur_rot_R, cur_rot_L);
				
				// 前回値との差分を計算
				dif_rot_R = (float)cur_rot_R - (float)pre_rot_R;
				dif_rot_L = (float)cur_rot_L - (float)pre_rot_L;
				
				// 開始からの左右輪の駆動距離[mm]
				dist_R += (WHEEL_RADIUS*PI)*(dif_rot_R/180);
				dist_L += (WHEEL_RADIUS*PI)*(dif_rot_L/180);
				print_new_line("dist_R = %f, L = %f", dist_R, dist_L);
				
				// 現在値を前回値として保存
				pre_rot_R = cur_rot_R;
				pre_rot_L = cur_rot_L;
				
				abs_dif_rot_L = abs(dif_rot_L);
				abs_dif_rot_R = abs(dif_rot_R);
				if( abs_dif_rot_L > abs_dif_rot_R){
					if( abs_dif_rot_L < 5 ){ gain_L = 1;}
					else{ gain_L = 1 - (abs_dif_rot_L - abs_dif_rot_R) / abs_dif_rot_L ;}
					gain_R = 1; 
				} else { // dif_rot_L * dif_rot_L <= dif_rot_R * dif_rot_R
					if( abs_dif_rot_R < 5 ){ gain_R = 1;}
					else{ gain_R = 1 - (abs_dif_rot_R - abs_dif_rot_L) / abs_dif_rot_R;}
					gain_L = 1;
				}
//				print_new_line("gain_R = %f, L = %f", gain_R, gain_L);
				
				duty_L = power * gain_L;
				duty_R = power * gain_R;
				
				if(deg < 0) { volante->setPWM(-duty_L, duty_R); }// ccw : deg -> -
				else        { volante->setPWM(duty_L, -duty_R); }// cw  : deg -> +
				tslp_tsk(50);
			}

			// 終了処理
//			volante->stop();
//			ev3_speaker_play_tone(NOTE_D4,100);

			print_new_line("Spin_Turn_end");
			return 0;
		}

	private:
		vector<int> type_deg;
		vector<int> type_power;
};

#endif // SPIN_TURN_H_
