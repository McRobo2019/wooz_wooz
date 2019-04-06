
#ifndef GLAY_SEARCHER_H_
#define GLAY_SEARCHER_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string.h>
#include "TraceController.h"
#include "common.h"

class GlaySearcher : public TraceController {

	public:
		GlaySearcher(LineMonitor *linemon, Volante *vlnt):
			TraceController(linemon, vlnt){
			
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
			thresh_ref = 0;
			found_times = 0;
			ang_l = 0;
			ang_r = 0;
//			pos_ave = 0;
			
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
					} else if ( line[0] >= '0' && line[0] <= '9' ) {
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
					} else if ( line[0] >= 'a' && line[0] <= 'z' ) {
						char *key = strtok(line, "=");
						char *num = strtok(NULL, "=");
						float val = atof(num);
						
						print_new_linef("key = %s", key);
						print_new_linef("val = %f", val);
						
						if(!strcmp(key,"tgt_black"))        { tgt_black   = val; }
						else if(!strcmp(key,"tgt_gray"))    { tgt_gray    = val; }
						else if(!strcmp(key,"tgt_white"))   { tgt_white   = val; }
						else if(!strcmp(key,"trace_length")){ trace_length= val; }
						else if(!strcmp(key,"thresh_ref"))  { thresh_ref  = val; }
						else if(!strcmp(key,"found_times")) { found_times = val; }
					} else {
						;
					}
				}
			}
			fclose(fp);
			
			for (unsigned int i = 0; i < control_param.size(); i++) {
				print_new_linef("v  = %f\n", control_param[i].v);
				print_new_linef("Kp = %f\n", control_param[i].pid.Kp);
				print_new_linef("Ki = %f\n", control_param[i].pid.Ki);
				print_new_linef("Kd = %f\n", control_param[i].pid.Kd);
			}
		}
	
	
		virtual int  set_type(int type) {
			search_type = type;
			volante->set_pid(control_param[type].pid);
			volante->set_speed(control_param[type].v);
			switch(type) {
				case 0 : line_target = tgt_black; break;
				case 1 : line_target = tgt_gray ; break;
				case 2 : line_target = tgt_white; break;
				default : break;
			}
			return 0;
		}


		virtual int drive() {
			
			if(drive_call_count == 0) {
				GlaySearcher::DriveStart();
				print_new_line("Glay Start");
			}
			
			ang_l = volante->get_angle_wheel_l() - start_ang_l;
			ang_r = volante->get_angle_wheel_r() - start_ang_r;
			
			curpos  = linemonitor->getPosition();
			
			GlaySearcher::setLog();
			
			drive_call_count++;
			
			print_new_line("cur_pos,%d",pos_log[0]);
			print_new_line("call_count %d",drive_call_count);
			print_new_line("search_typ %d",search_type);
			GlaySearcher::checkNode();
			print_new_line("pos_ave,%f",pos_ave);
			print_new_line("found_count,%d",found_count);
			
			if(search_type == 3) {
				if(GlaySearcher::checkNode() > 0) {
					print_new_line("Glay End");
					volante->stop();
					GlaySearcher::DriveEnd();
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
			} else if( GlaySearcher::checkStraight() > 0 ) {
				GlaySearcher::set_type(3);
				volante->setMaxSteer((int)20);
			}
			return(0);
		}
	
private:
	// drive呼び出し1回目の処理
	int DriveStart() {
		target = ( linemonitor->getTarget()*2 * line_target );
		start_ang_l = volante->get_angle_wheel_l();
		start_ang_r = volante->get_angle_wheel_r();
		return(0);
	}
	
	// drive終了時の処理
	int DriveEnd() {
		drive_call_count = 0;
		volante->reset_power();
		volante->setMaxSteer((int)200);
		return(0);
	}
	
	
	// ステア、タイヤ角、センサ値のログをとる
	// [0]から新しい順に入れていく（[0]は常に最新値）
	int setLog() {
		for(int i=LogNum-1; i>0; i--){
			pos_log[i]   = pos_log[i-1];
		}
		pos_log[0]   = (int)curpos;
		return(0);
	}
	
	// 与えられた配列のうち若いnum個を除いた平均を出す
	float LogAve(int ave[],int num = 0) {
		log_sum = 0;
		for(int i=num; i<LogNum; i++) {
			log_sum += (float)ave[i];
		}
		return(log_sum / (LogNum-num));
	}
	
	// 交点を検出する
	int checkNode() {
		// 変化点の数を数える
		node_point = 0;
		
		// 一度も交点を検出していない場合は最新の値とその他の平均値を比較し、
		// 一度交点を検出している場合は、交点を検出した際の値と比較する
		if( found_count == 0 || found_count > LogNum) {
			pos_ave = GlaySearcher::LogAve(pos_log,1);
		}
		
		if( abs(pos_log[0] - pos_ave) > thresh_ref ) { node_point++; }
		
		// ref値が閾値にかからなければ、連続灰色検出回数をリセットする
		if( node_point > 0) { found_count++; }
		else                { found_count=0; }
		
		// 連続交点検出回数が所定の値を超えたら交点を検出したとする
		if( found_count >= found_times ) { return(1); }
		else                             { return(0); }
		print_new_line("found_count %d",found_count);
	}
	
	// 左右輪の走行距離がtrace_lengthを超えると交点検出タイプに移行
	int checkStraight() {
		if( ((ang_l + ang_r)*PI*WHEEL_RADIUS/360) >  trace_length ) {
			print_new_line("run_trace_length %d",(int)((ang_l + ang_r)*PI*WHEEL_RADIUS/360));
			return(1);
		}
		return(0);
	}
	
	float target;
	int   search_type;
	int   trace_type;
	int   drive_call_count;
	int   start_ang_l;
	int   start_ang_r;
	int   found_count;
	float log_sum;
	int   pos_log[LogNum];
	float tgt_black;
	float tgt_gray;
	float tgt_white;
	float trace_length;
	float line_target;
	float ang_l;
	float ang_r;
	float thresh_ref;
	float found_times;
	float pos_ave;
	
	int node_point;
	
};

#endif // GLAY_SEARCHER_H_


