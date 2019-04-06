#ifndef POSITION_ESTIMATER_H_
#define POSITION_ESTIMATER_H_

#include "Motor.h"
#include "Util.h"
#include "common.h"

using namespace ev3api;

class PosEstimator {

	public:
		PosEstimator(Motor &left, Motor &right);
		Position getPosition();
		Position compute();
		float getMilage();
		int resetMilage(float surplus);
		int setMilage(float set_milage);

	private:
		Motor&   left_motor;
		Motor&   right_motor;
		Angle    angle_diff;     //１制御サイクルの回転角度
		Angle    angle_cur;      //現在の回転角度
		Angle    angle_pre;      //前回の回転角度
		Distance wheel_distance; //車輪の駆動距離
		float    round_trip;     //移動距離
		float    milage;         //移動距離(後退含む)
		float    theta_rad;      //回転角
		float    direction;      //方向
		Position position;       //現在位値
		float    vel_vector;     //移動ベクトル
};

#endif //POSITION_ESTIMATER_H_
