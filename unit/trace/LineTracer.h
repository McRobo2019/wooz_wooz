#ifndef LINE_TRACER_H_
#define LINE_TRACER_H_

#include "ev3api.h"
#include "Motor.h"
#include "Volante.h"
#include "LineMonitor.h"
#include "TraceController.h"
#include "StraightController.h"
#include "CurveController.h"
#include "EdgeChanger.h"
#include "NodeSearcher.h"
#include "GlaySearcher.h"
#include "BackController.h"
#include "Switcher.h"

using namespace ev3api;

class LineTracer {

	public:
		explicit LineTracer(Motor& left, Motor& right, LineMonitor* linemon,  Switcher* swtchr = NULL);
		int setSwitcher(Switcher* swtchr);
		int calibrate();
		int drive();
		int set_move_mode(int mode, int type);
		int reverse_steer(bool flag);
		int stop();
		float getLastPower();
		int setIniPower(float power_ini);

	private:
		Switcher* switcher;
		Volante* volante;
		TraceController *tracecon;
		StraightController *straight;
		CurveController *curve;
		EdgeChanger *edge;
		NodeSearcher *node;
		GlaySearcher *glay;
		BackController *back;
};

#endif // LINE_TRACER_H_
