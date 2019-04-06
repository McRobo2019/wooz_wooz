#include "LineTracer.h"
#include "Logger.h"

LineTracer::LineTracer(Motor& left, Motor& right, LineMonitor* linemon, Switcher* swtchr) : switcher(swtchr){
	volante  = new Volante(left, right);
	straight = new StraightController(linemon, volante);
	curve    = new CurveController(linemon, volante);
	edge     = new EdgeChanger(linemon, volante);
	node     = new NodeSearcher(linemon, volante);
	glay     = new GlaySearcher(linemon, volante);
	back     = new BackController(linemon, volante);
	tracecon = straight;
}

int LineTracer::setSwitcher(Switcher* swtchr){
	switcher = swtchr;
	return 0;
}

int LineTracer::calibrate() {
	return tracecon->calibrate();
}

int LineTracer::drive() {
	return tracecon->drive();
}

int LineTracer::reverse_steer(bool flag) {
	volante->reverse_steer(flag);
	return(0);
}

int LineTracer::set_move_mode(int mode, int type) {

	if ( mode == 0 ) {
		tracecon = straight;
	} else if ( mode == 1 ) {
		tracecon = curve;
	} else if ( mode == 2 ) {
		tracecon = edge;
	} else if ( mode == 3 ) {
		tracecon = node;
	} else if ( mode == 4 ) {
		tracecon = glay;
	} else if ( mode == 5 ) {
		tracecon = back;
	} else {
		print_new_linef("ERROR: LineTracer: undefined mode %d", mode);
	}
	tracecon->set_type(type);
	print_new_line("setmode %d",mode);

	return 0;
}

int LineTracer::stop() {
	print_new_line("LineTrace stop");
	return tracecon->stop();
}

float LineTracer::getLastPower() {
	return( volante->getPower() );
}

int LineTracer::setIniPower(float power_ini) {
	volante->setPower(power_ini);
	return(0);
}

