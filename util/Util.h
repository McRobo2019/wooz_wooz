#ifndef UTIL_H_
#define UTIL_H_

// 座標
class Position {
	public:
		float x;
		float y;
		Position() {
  			x = 0.0;
			y = 0.0;
		}
};

// モータ回転角
class Angle {
	public:
		int l;
		int r;
		int c;
		Angle() {
			l = 0.0;
			r = 0.0;
			c = 0.0;
			
		}
};

// 移動距離
class Distance {
	public:
		float l;
		float r;
		float c;
		Distance() {
			l = 0.0;
			r = 0.0;
			c = 0.0;
		}
};

// RGB値
class color {
	public:
		float r;
		float g;
		float b;
		color() { 
			r = 0;
			g = 0;
			b = 0;
		}
};

#endif // UTIL_H_

