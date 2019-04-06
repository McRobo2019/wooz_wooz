
#ifndef NODE_SEARCHER_H_
#define NODE_SEARCHER_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string.h>
#include "TraceController.h"
#include "Util.h"
#include "common.h"
#include "Timer.h"



class NodeSearcher : public TraceController {

	public:
		NodeSearcher(LineMonitor *linemon, Volante *vlnt):
			TraceController(linemon, vlnt){
			
			search_type = 0;
			pre_search_type = 0;
			trace_type = 0;
			drive_call_count = 0;
			found_count = 0;
			log_sum = 0;
			start_ang_l = 0;
			start_ang_r = 0;
			tgt_black = 0;
			tgt_gray = 0;
			tgt_white = 0;
			trace_length = 0;
			line_target = 0;
			found_times = 0;
			ang_l = 0;
			ang_r = 0;
			end_flag = 0;
			
			char *key = NULL;
			char *num = NULL;
			float val = 0;
				
			FILE *fp;
			if (( fp = fopen("/ev3rt/def/node_pidv.def", "r")) == NULL) {
				printf("can't read file\n");
			} else {
				char line[MAX_STR] = {0};
				while (fgets(line, sizeof(line), fp)) {
					PidMode mode;
					if ( line[0] == '#'  ) {
						continue;
					} else if ( line[0] == '\n' ) {
						continue;
					} else if ( line[0] >= '0' && line[0] <='9' ) {
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
							// print_new_line("pidv OK");
						}
						control_param.push_back(mode);
					} else if( line[0] >= 'a' && line[0] <= 'z' ) {
						key = strtok(line, "=");
						num = strtok(NULL, "=");
						val = atof(num);
						
						print_new_linef("key = %s", key);
						print_new_linef("val = %f", val);
						
						if(!strcmp(key,"tgt_black"))             { tgt_black     = val; }
						else if(!strcmp(key,"tgt_gray"))         { tgt_gray      = val; }
						else if(!strcmp(key,"tgt_white"))        { tgt_white     = val; }
						else if(!strcmp(key,"trace_length"))     { trace_length  = val; }
						else if(!strcmp(key,"thresh_red"))       { thresh.r      = val; }
						else if(!strcmp(key,"thresh_green"))     { thresh.g      = val; }
						else if(!strcmp(key,"thresh_blue"))      { thresh.b      = val; }
						else if(!strcmp(key,"thresh_glay_red"))  { thresh_glay.r = val; }
						else if(!strcmp(key,"thresh_glay_green")){ thresh_glay.g = val; }
						else if(!strcmp(key,"thresh_glay_blue")) { thresh_glay.b = val; }
						else if(!strcmp(key,"found_times"))      { found_times   = val; }
						else if(!strcmp(key,"red_max"))          { color_max.r   = val; }
						else if(!strcmp(key,"green_max"))        { color_max.g   = val; }
						else if(!strcmp(key,"blue_max"))         { color_max.b   = val; }
					} else {
						;
					}
				}
			}
				if(color_max.r == 0) { color_max.r = 300; }
				if(color_max.g == 0) { color_max.g = 450; }
				if(color_max.b == 0) { color_max.b = 600; }
					
			fclose(fp);
			
			for (unsigned int i = 0; i < control_param.size(); i++) {
				print_new_linef("v  = %f", control_param[i].v);
				print_new_linef("Kp = %f", control_param[i].pid.Kp);
				print_new_linef("Ki = %f", control_param[i].pid.Ki);
				print_new_linef("Kd = %f", control_param[i].pid.Kd);
			}
		}
	
	
		virtual int set_type(int type) {
			search_type = type;
			volante->set_pid(control_param[type].pid);
			volante->set_speed(control_param[type].v);
			switch(type) {
				case 0 : line_target = tgt_black; break;
				case 1 : line_target = tgt_gray ; break;
				case 2 : line_target = tgt_white; break;
				case 4 : drive_call_count = 0;    break;
				default : break;
			}
			return 0;
		}


		virtual int drive() {
			// リセットモードの場合はdriveされたらシナリオを次に進める
			if(search_type == 4) {
				end_flag = 0;
				return(1);
			} else {
				;
			}
			
			// 終了フラグが立っている間はdriveを実行しない
			if(end_flag > 0) {
				return(0);
			} else {
				;
			}
			
			// モードセット後初めて実行される場合は初期値をセットする
			if(drive_call_count < 1) {
				NodeSearcher::DriveStart();
				print_new_line("Node Start");
			}
			
			
			ang_l = volante->get_angle_wheel_l() - start_ang_l;
			ang_r = volante->get_angle_wheel_r() - start_ang_r;
			
			curpos  = linemonitor->getPosition();
			linemonitor->getRGB(&rgb_val);
			NodeSearcher::Normalization();
			
			NodeSearcher::setLog();
			
			drive_call_count++;
			
				print_new_line("call_count %d",drive_call_count);
				print_new_line("rgb.r.g.b,%d,%d,%d",red_log[0],grn_log[0],blu_log[0]);
//				Crosschecker::checkNode();
				print_new_line("color_ave.r.g.b,%f,%f,%f",color_ave.r, color_ave.g, color_ave.b);

			
			
			if(search_type == 3) {
				if(NodeSearcher::checkNode() > 0) {
					print_new_line("Node_End");
					volante->stop();
					NodeSearcher::DriveEnd();
					return(1);
				}
			}
			
			volante->steerPID(target - curpos);
			volante->calc_speed_pid();
			volante->drive();
			
			if( search_type == 3) {
				return(0);
			} else if( drive_call_count <= LogNum ) {
				return(0);
			} else if( checkStraight() > 0 ) {
				NodeSearcher::set_type(3);
//				volante->setMaxSteer((int)10);
			} else {
				;
			}
			return(0);
		}
	
private:
	// drive呼び出し1回目の処理
	int DriveStart() {
		target = ( linemonitor->getTarget()*2 * line_target );
		start_ang_l = volante->get_angle_wheel_l();
		start_ang_r = volante->get_angle_wheel_r();
		pre_search_type = search_type;
		return(0);
	}
	
	// drive終了時の処理
	int DriveEnd() {
		drive_call_count = 0;
		volante->reset_power();
		volante->setMaxSteer((int)200);
		end_flag = 1;
		return(0);
	}
	
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
	
	// ステア、タイヤ角、センサ値のログをとる
	// [0]から新しい順に入れていく（[0]は常に最新値）
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
			color_ave.r = NodeSearcher::LogAve(red_log,1);
			color_ave.g = NodeSearcher::LogAve(grn_log,1);
			color_ave.b = NodeSearcher::LogAve(blu_log,1);
		}
//		print_new_line("pre_search_type %d",pre_search_type);
		if( pre_search_type == 2) {
			// 白トレース（黒交点検出）時はセンサ値が小さくなる方向なので、式を反転させる
			if( (if_red = ((float)color_ave.r - red_log[0])) > thresh.r ) { node_point++; }
			if( (if_grn = ((float)color_ave.g - grn_log[0])) > thresh.g ) { node_point++; }
			if( (if_blu = ((float)color_ave.b - blu_log[0])) > thresh.b ) { node_point++; }
		} else if(pre_search_type == 1) {
			// 灰トレース（色交点検出）時はセンサ値が各色によって上下するため、絶対値で判断する
			if( (if_red = fabsf((float)red_log[0] - color_ave.r)) > thresh_glay.r ) { node_point++; }
			if( (if_grn = fabsf((float)grn_log[0] - color_ave.g)) > thresh_glay.g ) { node_point++; }
			if( (if_blu = fabsf((float)blu_log[0] - color_ave.b)) > thresh_glay.b ) { node_point++; }
		} else {
			// 黒トレース中は全色色に対しセンサ値が大きくなる方向のため、現在値から平均値をを引く
			if( (if_red = ((float)red_log[0] - color_ave.r)) > thresh.r ) { node_point++; }
			if( (if_grn = ((float)grn_log[0] - color_ave.g)) > thresh.g ) { node_point++; }
			if( (if_blu = ((float)blu_log[0] - color_ave.b)) > thresh.b ) { node_point++; }
		}
//		print_new_line("node_point %d",node_point);
		
		// R,G,Bのうち、1つも閾値にかからない場合は黒、3つともかかる場合は白と判断し、
		// 連続交点検出回数をリセットする
		if( node_point > 0 /*&& node_point < 3*/) { found_count++; }
		else                                  { found_count=0; }
		
		// 連続交点検出回数が所定の値を超えたら交点を検出したとする
		if( found_count >= found_times ) { return(1); }
		else                             { return(0); }
//		print_new_line("found_count %d",found_count);
	}
	
	// 左右輪の走行距離がtrace_lengthを超えると交点検出タイプに移行
	int checkStraight() {
		if( ((ang_l + ang_r)*PI*WHEEL_RADIUS/360) >  trace_length ) {
			return(1);
		}
		return(0);
	}
	
	float target;
	int   search_type;
	int   pre_search_type;
	int   trace_type;
	int   drive_call_count;
	int   start_ang_l;
	int   start_ang_r;
	int   found_count;
	float log_sum;
	int   pos_log[LogNum];
	int   red_log[LogNum];
	int   grn_log[LogNum];
	int   blu_log[LogNum];
	float tgt_black;
	float tgt_gray;
	float tgt_white;
	float trace_length;
	float line_target;
	float ang_l;
	float ang_r;
	float found_times;
	rgb_raw_t rgb_val;
	color thresh;
	color thresh_glay;
	color color_max;
	color color_val;
	color color_ave;
	
	int node_point;
	int end_flag;
	
	// Log取り用変数
	float if_red;
	float if_grn;
	float if_blu;
};

#endif // NODE_SEARCHER_H_


