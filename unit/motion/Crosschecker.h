#ifndef CROSSCHECKER_H_
#define CROSSCHECKER_H_

#include <cmath>
#include "common.h"
#include "Motion.h"
#include "Util.h"
#include "Logger.h"

using namespace std;


class Crosschecker: public Motion {
	
	public:
		// 距離とvelは引数がない場合は以下となる
		Crosschecker(Volante *vlnt, StopWatch *watch, ColorSensor& sensor)
				: Motion(vlnt, watch), colorSensor(sensor){
			
			FILE *fp;
			if (( fp = fopen("/ev3rt/def/cross_checker.def", "r")) == NULL) {
				printf("can't read file: cross_checker.def\n");
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
								case 2: type_dist.push_back( atof(token) ); break;
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
			
			char *key = NULL;
			char *num = NULL;
			float val = 0;
			
			FILE *fp2;
			if (( fp2 = fopen("/ev3rt/def/node_pidv.def", "r")) == NULL) {
				printf("can't read file\n");
			} else {
				char line[MAX_STR] = {0};
				while (fgets(line, sizeof(line), fp2)) {
					if ( line[0] == '#'  ) {
						continue;
					} else if ( line[0] == '\n' ) {
						continue;
					} else if ( line[0] >= '0' && line[0] <='9' ) {
						continue;
					} else if( line[0] >= 'a' && line[0] <= 'z' ) {
						key = strtok(line, "=");
						num = strtok(NULL, "=");
						val = atof(num);
						
						print_new_linef("key = %s", key);
						print_new_linef("val = %f", val);
						
						if(!strcmp(key,"thresh_red"))            { thresh.r      = val; }
						else if(!strcmp(key,"thresh_green"))     { thresh.g      = val; }
						else if(!strcmp(key,"thresh_blue"))      { thresh.b      = val; }
						else if(!strcmp(key,"found_times"))      { found_times   = val; }
						else if(!strcmp(key,"red_max"))          { color_max.r   = val; }
						else if(!strcmp(key,"green_max"))        { color_max.g   = val; }
						else if(!strcmp(key,"blue_max"))         { color_max.b   = val; }
					} else {
						;
					}
				}
			}
			fclose(fp2);
		}


		// driveをコールする前にset_typeする必要がある
		virtual int drive() {
			print_new_line("CrossCheck_start");
			
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
			
			volante->set_speed(type_vel[type]);
			drive_call_count = 0;
			
			while( distance_mm > (dist_R+dist_L)/2 ) {
				/* 交点検出 */
				
				colorSensor.getRawColor(rgb_val);
				Crosschecker::Normalization();
				
				Crosschecker::setLog();
				
				drive_call_count++;
				
//				print_new_line("call_count %d",drive_call_count);
//				print_new_line("rgb.r.g.b,%d,%d,%d",red_log[0],grn_log[0],blu_log[0]);
//				Crosschecker::checkNode();
//				print_new_line("color_ave.r.g.b,%f,%f,%f",color_ave.r, color_ave.g, color_ave.b);
			
				if( drive_call_count > LogNum){
					if(Crosschecker::checkNode() > 0) {
						print_new_line("CrossCheck_End");
						volante->stop();
						volante->reset_power();
						break;
					}
				}
				
				
				
				/* 走行制御 */
				
				// 速度制御追加 0903 tamamru
				duty = volante->calc_speed_pid();
				// 現在のモーターの回転角を取得
				cur_rot_R = volante->get_angle_wheel_r();
				cur_rot_L = volante->get_angle_wheel_l();
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
				duty_L = duty * gain_L;
				duty_R = duty * gain_R;
				volante->setPWM(duty_L, duty_R);
				
				
				
				tslp_tsk(30);
				
			}
			
			// 終了処理
//			volante->stop();
//			ev3_speaker_play_tone(NOTE_D4,100);

			print_new_line("Crosschecker_end");
			return 0;
		}

private:
	
	
	// 各色の値を正規化する
	int Normalization() {
		color_val.r = (float)rgb_val.r;
		color_val.g = (float)rgb_val.g;
		color_val.b = (float)rgb_val.b;
		color_val.r = color_val.r / color_max.r * 255.0;
		color_val.g = color_val.g / color_max.g * 255.0;
		color_val.b = color_val.b / color_max.b * 255.0;
		return(0);
	}
		
	int setLog() {
		for(int i=LogNum-1; i>0; i--){
			red_log[i]   = red_log[i-1];
			grn_log[i]   = grn_log[i-1];
			blu_log[i]   = blu_log[i-1];
		}
		red_log[0]   = (int)color_val.r;
		grn_log[0]   = (int)color_val.g;
		blu_log[0]   = (int)color_val.b;
		return(0);
	}
	
	// 与えられた配列のうち若いnum個を除いた平均を出す
	float LogAve(int ave[],int num = 0) {
		log_sum = 0;
		for(int i=num; i<LogNum; i++) {
			log_sum += (float)ave[i];
		}
//		print_new_line("log_sum,%f",log_sum);
		return(log_sum / (LogNum-num));
	}
	
	// 交点を検出する
	int checkNode() {
		// 変化点の数を数える
		node_point = 0;
		
		// 一度も交点を検出していない場合は最新の値とその他の平均値を比較し、
		// 一度交点を検出している場合は、交点を検出した際の値と比較する
		if( found_count == 0 || found_count > LogNum) {
			color_ave.r = Crosschecker::LogAve(red_log,1);
			color_ave.g = Crosschecker::LogAve(grn_log,1);
			color_ave.b = Crosschecker::LogAve(blu_log,1);
		}
		if( ((float)color_ave.r - red_log[0]) > thresh.r ) { node_point++; }
		if( ((float)color_ave.g - grn_log[0]) > thresh.g ) { node_point++; }
		if( ((float)color_ave.b - blu_log[0]) > thresh.b ) { node_point++; }
		
		if( node_point > 1 ) { found_count++; }
		else                 { found_count=0; }
		
		// 連続交点検出回数が所定の値を超えたら交点を検出したとする
		if( found_count >= found_times ) { return(1); }
		else                             { return(0); }
	}
	
	ColorSensor& colorSensor;
	
	vector <int> type_dist;
	vector <float> type_vel;
	
	int   drive_call_count;
	int   found_count;
	float log_sum;
	int   red_log[LogNum];
	int   grn_log[LogNum];
	int   blu_log[LogNum];
	float found_times;
	rgb_raw_t rgb_val;
	color thresh;
	color color_max;
	color color_val;
	color color_ave;
	int   node_point;
	
};

#endif // CROSSCHECKER_H_
