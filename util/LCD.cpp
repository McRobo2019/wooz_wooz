#include <cstdio>
#include <cstdarg>
#include <cstring>
#include "LCD.h"

#define MAX_LINE (8)


int32_t LCD::cur_line;
Font *LCD::font;
char LCD::buffer[100];

LCD::LCD() { 
	clear();
	font = new Font(EV3_FONT_MEDIUM);
	cur_line = 0;
}

void LCD::clear() {
	ev3_lcd_fill_rect(0, 0, WIDTH, HEIGHT, WHITE);
	cur_line = 0;
}
void LCD::clearRect(Coordinate *crd) {
	ev3_lcd_fill_rect(crd->x, crd->y, WIDTH, HEIGHT, WHITE);
}	

void LCD::show(char *form, ...) {

	memset(buffer, 0, 100);

	va_list arg;
	va_start(arg, form);
	vsprintf(buffer, form, arg);
	va_end(arg);
	ev3_lcd_draw_string(buffer, 0, cur_line*font->height);
	if (cur_line == MAX_LINE) {
			cur_line = 0;
	} else {
			cur_line++;
	}
}

