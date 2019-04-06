#ifndef STOP_H_
#define STOP_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "common.h"
#include "Motion.h"
#include "Logger.h"
#include "Timer.h"

class Stop: public Motion {

	public:
		Stop(Volante *vlnt, StopWatch *watch):Motion(vlnt, watch) { ; }

		virtual int drive() {
			
			print_new_line("!!!----------------STOP----------------!!!");
//			print_new_line("R_power:%f L_Poewr:%f",volante->getPower_r(),volante->getPower_l());
			if(type < 1) {
				// typeが0の場合、時間は関係なく100回ループを回るまで停止する
				CountStop(100);
			} else {
				// typeが0以外の場合、type*100ms間停止する
				TimeStop((int)timer_now(), type*100);
			}
//			ev3_speaker_play_tone(NOTE_D4,100);
			print_new_line("stop_end");
			return(0);
		}
	
	private:
		
		int CountStop(int stop_count) {
			for( int i=0; i < stop_count; i++ ) {
				volante->stop();
				volante->reset_power();
				print_new_line(" %d, R_power:%f, L_poewr:%f",i,volante->getPower_r(),volante->getPower_l());
			}
			return(0);
		}
		
		int TimeStop(int start_time, int stop_time) {
			while(1){
				volante->stop();
				volante->reset_power();
				print_new_line("R_power:%f, L_poewr:%f",volante->getPower_r(),volante->getPower_l());
					
				if( ((int)timer_now()-start_time) >= stop_time ) {
					break;
				}
			}
			return(0);
		}
		
};

#endif // STOP_H_
