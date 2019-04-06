#ifndef LCD_H_
#define LCD_H_

#include "ev3api.h"

using namespace std;

#define lcd_clear LCD::clear
#define lcd_print(fmt, ...) { LCD::show((char*)fmt, ##__VA_ARGS__); }

class Coordinate {

	public:
		int32_t x;
		int32_t y;

		Coordinate() {
			x = 0;
			y = 0;
		}
};

class Font {

		public:
		int32_t width;
		int32_t height;

		void set(lcdfont_t font) {
			ev3_lcd_set_font(font);
			ev3_font_get_size(font, &width, &height);
		}
		Font(lcdfont_t font) {
			set(font);
		}
};

class LCD {

	public:
		explicit LCD();
		static void clear();
		static void clearRect(Coordinate *crd);
		static void show(char *form, ...);

	private:
		static const int32_t WIDTH  = EV3_LCD_WIDTH;
		static const int32_t HEIGHT = EV3_LCD_HEIGHT;
		static const lcdcolor_t WHITE = EV3_LCD_WHITE;
		static const lcdcolor_t BLACK = EV3_LCD_BLACK;
		static Coordinate org;
		static Coordinate cur;
		static int32_t cur_line;
		static Font *font;
		static char buffer[100];
};

#endif // LCD
