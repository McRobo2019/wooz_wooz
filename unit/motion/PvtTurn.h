#ifndef PVT_TURN_H_
#define PVT_TURN_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "common.h"
#include "Motion.h"
#include "Logger.h"

class PvtTurn: public Motion {

	// TODO: ターンはピボットターンとして分離する
	// 組み合わせは別コマンドとして定義するため

	enum {
		TURN_LEFT = 0,
		TURN_RIGHT
	};
	
	public:
		PvtTurn(Volante *vlnt, StopWatch *watch):Motion(vlnt, watch) {

			FILE *fp;

			if (( fp = fopen("/ev3rt/def/pivot_turn.def", "r")) == NULL) {
				printf("can't read file: pivot_turn.def\n");
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
								case 1: type_tire.push_back( atoi(token) ); break;
								case 2: type_power.push_back( atoi(token) ); break;
								default : break;
							}
						}
					}
				}
			}
			for (unsigned int i = 0; i < type_deg.size(); i++) {
				print_new_linef("%d, deg = %d[mm], power = %d", type_deg[i], type_power[i]);
			}
			fclose(fp);
		}


		virtual int drive() {
			
			print_new_line("Pivot_turn_start");
			
//			int distance_mm = 0;   // 指定角度から算出した片輪が進む円弧の長さ
			int rot_deg = 0;  // 必要角度回るためのタイヤの回転角度
			
			int deg   = type_deg[type];
			int tire  = type_tire[type];
			int power = type_power[type];
			
			// TODO:他の関数と共通の表現にする
			// できればキャンセルされる計算は除く
//			distance_mm = (TRED_WIDTH * PI* deg) / 180;
//			rot_deg = (int)(180*distance_mm)/(PI*WHEEL_RADIUS)*1.06;
			rot_deg = (int)( (TRED_WIDTH * deg) / WHEEL_RADIUS *1.06 );
			
			if     ( tire ==  1 ) { volante->rotate_wheel_r(rot_deg, power, true); }
			else if( tire == -1 ) { volante->rotate_wheel_l(rot_deg, power, true); }
			
			// 終了処理
//			volante->stop();
//			volante->reset_power();
//			ev3_speaker_play_tone(NOTE_D4,100);
			print_new_line("Pivot_Turn_end");
			return 0;
		}

	private:
		vector<int> type_deg;
		vector<int> type_tire;
		vector<int> type_power;
};

#endif // PVT_TURN_H_
