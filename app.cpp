#include "app.h"
#include "Logger.h"
#include "BatteryMonitor.h"
#include "TouchStarter.h"
#include "LineMonitor.h"
#include "LineTracer.h"
#include "MotionSequencer.h"
#include "Driver.h"
#include "SoundPlayer.h"
#include "Timer.h"
#include "LCD.h"
#include "ColorIdentify.h"

void * __dso_handle;

ColorSensor colorSensor(PORT_1);
TouchSensor touchSensor(PORT_4);
Motor leftMotor(PORT_D);
Motor rightMotor(PORT_A);
Motor armMotor(PORT_B);

int flag_exit_logger;

char **tmp;
static Timer           *timer;
static LCD             *lcd;
static BatteryMonitor  *batteryMonitor;
static TouchStarter    *touchStarter;
static LineTracer      *lineTracer;
static LineMonitor     *lineMonitor;
static MotionSequencer *motionSeq;      
static Driver          *driver;


static void user_system_create() {

	flag_exit_logger = 0;

	logger_init();

	timer          = new Timer();
	lcd            = new LCD();
	lcd_print("LCDOK");
	batteryMonitor = new BatteryMonitor();
	lcd_print("batteryOK");
	touchStarter   = new TouchStarter(touchSensor);
	lcd_print("touchStarterOK");
	lineMonitor    = new LineMonitor(colorSensor, touchSensor);
	lcd_print("lineMonitorOK");
	lineTracer     = new LineTracer(leftMotor, rightMotor, lineMonitor);
	lcd_print("lineTracerOK");
	motionSeq      = new MotionSequencer(leftMotor, rightMotor, armMotor, colorSensor);
	lcd_print("motionSeqOK");
	driver         = new Driver(leftMotor, rightMotor, lineTracer, motionSeq);
	lcd_print("driverOK");

	act_tsk(LOGGER_TASK);
	tslp_tsk(1000);
	//sound_set_volume(3);
}

void ev3_cyc_tracer(intptr_t exinf) {
	int state = driver->getChangeState();
	if ( state != 1 ) {
		act_tsk(TRACER_TASK);
	}
}

void main_task(intptr_t unused) {

	user_system_create();
	
	lcd_clear();
	
	// 起動時のバッテリ電圧を測定
	batteryMonitor->measure();
	float v = batteryMonitor->get_voltage();
	lcd_print("Vbat = %6.3f [V]", v);
	
	lineTracer->calibrate();
	
	// 右エッジスタート時はコメントアウトする
	//lineTracer->reverse_steer(true);
	// スイッチを押すとスタート
	lcd_print("Push Start");
	sound_play_chime2(NOTE_C4, 30);
	
//	print_new_line("befor_press");

	touchStarter->waitPressed();
	
// 	print_new_line("colorIdentify %d", colorIdentify->getUnderColor());
// 	print_new_line("colorIdentify %d", colorIdentify->getForwardColor());
// 	print_new_line("colorIdentify %d", colorIdentify->getUnderColor());
// 	print_new_line("colorIdentify %d", colorIdentify->getForwardColor());

	int flag_start_cyc = 1;

	// メインタスクの周期処理
	while (!touchStarter->isPressed()) {
//	tslp_tsk(500);
		print_new_line("driver process start  = %d", timer_now());
//		print_new_line("driver process = %d", timer_now());
		if ( driver->drive() ) {
			print_new_linef("scenarios end = %d", timer_now());
			break;
		}
		if (flag_start_cyc == 1) {
			// 周期タスクの起動
			ev3_sta_cyc(EV3_CYC_TRACER);
			flag_start_cyc = 0;
		}
		print_new_line("driver process end    = %d", timer_now());
		tslp_tsk(2);
	}
	//
	// 周期タスクの終了
	ev3_stp_cyc(EV3_CYC_TRACER);
	// ライントレーサの停止
	print_new_line("app stop");
	lineTracer->stop();
	
	lcd_print("\(^o^)/owata");


	sound_play_chime2(NOTE_C4, 30);
	sound_play_chime2(NOTE_D4, 30);
	sound_play_chime2(NOTE_E4, 30);
	sound_play_chime2(NOTE_F4, 30);

	print_new_linef("driver task exit  = %d", timer_now());

	// ロギングタスクの終了待ち
	flag_exit_logger = 1;
	tslp_tsk(100);
	FLGPTN flag_logger_exit; 
	wai_flg(FLAG_LOGGER_EXIT, 0x01, TWF_ANDW, &flag_logger_exit);
	logger_exit();

	// タスクの終了
	sound_play_chime2(NOTE_C4, 30);
	ext_tsk();
}

// ライントレース用のタスク
// 優先度が最高のため、[実行状態]になると必ず実行される
// 開始条件: 周期起動タスクから実行される
// 終了条件: 関数の内部処理(ステップ)が実行される
void tracer_task(intptr_t exinf) {
//	print_new_line("tracer task start = %d", timer_now());

//	print_new_linef("tracer task start = %d", timer_now());
	print_new_line("tracer_task_start %d", timer_now());
//	motionSeq->drive();
	if(lineTracer->drive()){
		driver->goNextScenario();
//		ev3_stp_cyc(EV3_CYC_TRACER);
	}
//	lcd_print("linetrace task %d", count_trace);
//	print_new_line("tracer task end = %d", timer_now());

	print_new_line("tracer task end = %d", timer_now());
	ext_tsk();
}

// Loggerクラスで保存されているログをSDカードに出力するためのタスク
// 優先度が最低のため、他のタスクが[待ち状態]にならないと実行されない
// 開始条件: プログラムの起動時から起床される
// 終了条件: メインタスクでフラグが設定されるまで実行
void logger_task(intptr_t exinf) {

//	print_new_linef("logger task start  = %d", timer_now());

	while (!flag_exit_logger) {
//		print_new_line("logger task = %d", timer_now());
		logger_dump();
	}

//	print_new_linef("logger task end  = %d", timer_now());

	// メインタスクへの終了イベント通知
	set_flg(FLAG_LOGGER_EXIT, 0x01);
	ext_tsk();
}
