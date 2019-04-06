#ifndef NML_TURN_H_
#define NML_TURN_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "common.h"
#include "Motion.h"
#include "Logger.h"
#include "Timer.h"

class Turn: public Motion {

	// TODO: ターンはピボットターンとして分離する
	// 組み合わせは別コマンドとして定義するため

	enum {
		TURN_LEFT = 0,
		TURN_RIGHT
	};
	
	public:
		Turn(Volante *vlnt, StopWatch *watch):Motion(vlnt, watch) {

			FILE *fp;

			if (( fp = fopen("/ev3rt/def/nml_turn.def", "r")) == NULL) {
				printf("can't read file: nml_turn.def\n");
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
					type_power.push_back( atoi(token) );
					int count = 1;
					while ( token != NULL ) {
						token = strtok( NULL, "," );
						if ( token != NULL ) {
							switch (count++) {
								case 1: type_deg_L.push_back( atoi(token) ); break;
								case 2: type_deg_R.push_back( atoi(token) ); break;
								default : break;
							}
						}
					}
				}
			}
			for (unsigned int i = 0; i < type_deg_R.size(); i++) {
				print_new_linef("%d, deg = %d[mm], power = %d\n", type_deg_R[i], type_deg_L[i]);
			}
			fclose(fp);
		}


		virtual int drive() {
			print_new_line("Normal_turn_start");

			int dist_R    = 0;   // 指定角度から算出した片輪が進む円弧の長さ
			int rot_deg_R = 0;   // 必要角度回るためのタイヤの回転角度
			int dist_L    = 0;   // 指定角度から算出した片輪が進む円弧の長さ
			int rot_deg_L = 0;  // 必要角度回るためのタイヤの回転角度

			int deg_R = type_deg_R[type];
			int deg_L = type_deg_L[type];
			int power = type_power[type];
			
			//float Battery_V = ev3_battery_voltage_mV()*0.001; // [V]

			// TODO:他の関数と共通の表現にする
			// できればキャンセルされる計算は除く
			dist_R = (TRED_WIDTH * PI* deg_R) / 180 * 1.06;
			rot_deg_R = (int)(180*dist_R)/(PI*WHEEL_RADIUS);
			dist_L = (TRED_WIDTH * PI* deg_L) / 180 * 1.06;
			rot_deg_L = (int)(180*dist_L)/(PI*WHEEL_RADIUS);
//			if     ( tire ==  1 ) pre_rot = volante->get_angle_wheel_l();
//			else if( tire == -1 ) pre_rot = volante->get_angle_wheel_r();
			
			if (abs(rot_deg_R)>abs(rot_deg_L))
			{
				volante->rotate_wheel_l(rot_deg_L, power, false);
				volante->rotate_wheel_r(rot_deg_R, power, true);
			} else
			{
				volante->rotate_wheel_r(rot_deg_R, power, false);
				volante->rotate_wheel_l(rot_deg_L, power, true);
			}
//			while( distance_mm * distance_mm > dist * dist ){

				// 現在のモーターの回転角を取得
//				cur_rot = volante->get_angle_wheel_r();
//				if     ( tire ==  1 ) cur_rot = volante->get_angle_wheel_l();
//				else if( tire == -1 ) cur_rot = volante->get_angle_wheel_r();
				
				// 前回値との差分を計算
//				dif_rot = (float)cur_rot - (float)pre_rot;
				
				// 開始からの左右輪の駆動距離[mm]
//				dist += (WHEEL_RADIUS*PI)*(dif_rot/180);
				
				// 現在値を前回値として保存
//				pre_rot = cur_rot;

//				if     ( tire ==  1 ) volante->setPWM(power, 0); // ccw : deg -> +
//				else if( tire == -1 ) volante->setPWM(0, power); // cw  : deg -> -
//				tslp_tsk(50);
//			}
			// 終了処理
//			volante->stop();
//			ev3_speaker_play_tone(NOTE_D4,100);

			print_new_line("Normal_Turn_end");
			return 0;
		}

	private:
		vector<int> type_deg_R;
		vector<int> type_deg_L;
		vector<int> type_power;
};

#endif // NML_TURN_H_
