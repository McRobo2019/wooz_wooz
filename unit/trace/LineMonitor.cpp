#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "LineMonitor.h"
#include "SoundPlayer.h"
#include "Logger.h"
#include "LCD.h"
#include "Timer.h"

LineMonitor::LineMonitor(ColorSensor &color, TouchSensor &touch):
	color_sensor(color),touch_sensor(touch) {
	white = 0;
	black = 0;
	FILE *fp;

	if( (fp = fopen("conf.ini","r")) == NULL ){

		print_new_line("ERROR: initial file can't open");

	} else {

		char line[100]={0};
		char *key  = NULL;
		char *num  = NULL;
		float val = 0;
		while(fgets(line, sizeof(line), fp)){
			key = strtok(line, "=");
			num = strtok(NULL, "=");
			val = atof(num);
			if(!strcmp(key,"line_target")) {
				target = val;
			}
		}
		fclose(fp);
	}

}

int LineMonitor::calibrate() {

	// カラーセンサの校正
	// 白部分の情報を取得
	sound_play_chime2(NOTE_C4, 100);
	//ev3_lcd_draw_string( "Push on White" , 0, currentFtHeight );
	
	lcd_print("touch on white");
	
	// タッチセンサの押下待ち
	while(!touch_sensor.isPressed());
	while(touch_sensor.isPressed());
	white = color_sensor.getBrightness();
	
	// 画面に表示
	lcd_print("White No. = %d", white);
//	ev3_lcd_draw_string( sNum, 0, currentFtHeight );
	// 黒部分の情報を取得
	sound_play_chime2(NOTE_C4, 100);

   	// 画面に黒の値を表示
// 	ev3_lcd_draw_string( "Push on Black" , 0, 2*currentFtHeight );
	
	lcd_print("touch on black");
	
	// タッチセンサの押下待ち
	while(!touch_sensor.isPressed());
	while(touch_sensor.isPressed());
	black = color_sensor.getBrightness();

	// 画面に表示
	lcd_print("Black No. = %d", black);
//	ev3_lcd_draw_string( sNum, 0, 2*currentFtHeight );
	sound_play_chime2(NOTE_C4, 100);

	print_new_line("sens_white:%d", white); 
	print_new_line("sens_black:%d", black); 
	print_new_line("target:%d", (white + black)/2 -black); 

	return (target = (white + black) / 2 - black);
}

float LineMonitor::getTarget() { 
	return (target);
}

int LineMonitor::getPosition() {
	print_new_line("Refrect %d", (int)color_sensor.getBrightness());
	print_new_line("time,%d", timer_now());
	return (color_sensor.getBrightness());
}

int LineMonitor::getColor() {
	print_new_line("ColorNum %d", (int)(color_sensor.getColorNumber()));
	return (0);
}

void LineMonitor::getRGB(rgb_raw_t *rgb) {
	print_new_line("time,%d", timer_now());
	color_sensor.getRawColor(rgb_val);
	print_new_line("time,%d", timer_now());
	rgb->r = rgb_val.r;
	rgb->g = rgb_val.g;
	rgb->b = rgb_val.b;
	print_new_line("time,%d", timer_now());
	print_new_line("Raw Color R %d",(int)rgb_val.r);
	print_new_line("Raw Color G %d",(int)rgb_val.g);
	print_new_line("Raw Color B %d",(int)rgb_val.b);
//	return 0;
}
