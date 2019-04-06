#ifndef MOTION_H_
#define MOTION_H_

#include "Volante.h"
#include "StopWatch.h"

struct DRIVE_MODE_LTR {
	enum {
		LINE_TRACE_STR = 0,
		LINE_TRACE_CRV,//1
		LINE_TRACE_EDG,//2
		LINE_TRACE_NOD,//3
		LINE_TRACE_GLY,//4
		LINE_TRACE_BCK,//5
		NUM
	};
};

struct DRIVE_MODE_MTN {
	enum {
		MOTION_FWD_TRVL = 10,
		MOTION_NML_TURN, //11
		MOTION_SPN_TURN, //12
		MOTION_PVT_TURN, //13
		MOTION_CRS_CHCK, //14
		MOTION_STOP, //15
		MOTION_COL_CHCK = 20,
		MOTION_EXP_CHCK, //21
		NUM
	};
};

class Motion {
	public:
		explicit Motion(Volante *vlnt, StopWatch *watch);
		virtual int set_type(int type);
		virtual int drive() = 0;
		int stop();

	protected:
		Volante *volante;
		StopWatch *swatch;
		int type;
};

#endif // MOTION_H_
