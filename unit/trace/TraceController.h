
#ifndef TRACE_CONTROLLER_H_
#define TRACE_CONTROLLER_H_

#include <vector>
#include "LineMonitor.h"
#include "Volante.h"
#include "Clock.h"
#include "Logger.h"


using namespace std;

class PidMode {
	public:
		float v;
		Pid pid;
		PidMode():v(0), pid(0,0,0){;}
};

class TraceController {

	public:
		TraceController(LineMonitor *linemon, Volante *vlnt);
		virtual int set_type(int type) = 0;
		virtual int drive() = 0;
		int calibrate();
		int stop();

	protected:
		LineMonitor *linemonitor;
		Volante *volante;
		int curpos;				//現在の反射値
		float line_target;		//ライントレースの狙いの反射値
		float edge_steer_angle;	//エッジ切り替えのステア角
		float edge_velocity;	//エッジ切り替えの車速
		int   edge_repeat_count;//エッジ切り替えの処理回数
		vector<PidMode> control_param;   
};

#endif // TRACE_CONTROLLER_H_
