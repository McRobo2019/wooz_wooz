#include <cmath>
#include "PosEstimator.h"
#include "Logger.h"

PosEstimator::PosEstimator(Motor &left, Motor &right):left_motor(left),right_motor(right) {
	round_trip   = 0.0;
	milage       = 0.0;
	theta_rad    = 0.0;
	direction    = 0.0;
	vel_vector   = 0.0;
}

Position PosEstimator::getPosition() {
	return position;
}

float PosEstimator::getMilage() {
	return(milage);
}

int PosEstimator::resetMilage(float surplus) {
	milage = surplus;
	angle_pre.l = left_motor.getCount();
	angle_pre.r = right_motor.getCount();
	return(0);
}

// 交点検出時強制目標距離達成用
int PosEstimator::setMilage(float set_milage) {
	milage = set_milage;
	return(0);
}

Position PosEstimator::compute() {

	angle_cur.l  = left_motor.getCount();
	angle_cur.r  = right_motor.getCount();
	
	angle_diff.l = angle_cur.l - angle_pre.l;
	angle_diff.r = angle_cur.r - angle_pre.r;
	
	wheel_distance.l = PI * WHEEL_RADIUS * angle_diff.l/180;
	wheel_distance.r = PI * WHEEL_RADIUS * angle_diff.r/180;
	
	round_trip += (wheel_distance.l + wheel_distance.r) / 2;
	milage += abs(wheel_distance.l + wheel_distance.r) / 2;
	
	theta_rad = (wheel_distance.l - wheel_distance.r) / TRED_WIDTH;
	direction +=  theta_rad / PI * 180;
	
	if ( theta_rad < 0.01 ) {
		vel_vector = wheel_distance.r;
	} else {
		vel_vector = sin(theta_rad) / cos(theta_rad/2) * (wheel_distance.r/theta_rad + TRED_WIDTH/2);
	}
	
	angle_pre.l  = angle_cur.l;
	angle_pre.r  = angle_cur.r;
	
	position.x +=  vel_vector * cos( (PI/2) - (theta_rad + direction * PI / 180) );
	position.y +=  vel_vector * sin( (PI/2) - (theta_rad + direction * PI / 180) );
	
	return position;
}
