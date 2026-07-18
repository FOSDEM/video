#include "display.h"
#include "channels.h"
#include "db_conversion.h"

#include <ST7735_t3.h>

#define TFT_DC         (12)
#define TFT_CSL        (10)
#define TFT_CSR        (31)
#define TFT_MOSI       (11)
#define TFT_RST        (24)
#define TFT_SCK        (13)

#define SCREEN_ADDRESS 0x3C
#define OLED_RESET     (-1)

ST7735_t3 display[] = {
	ST7735_t3(TFT_CSL, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST),
	ST7735_t3(TFT_CSR, TFT_DC, TFT_MOSI, TFT_SCK, 0),
};

void display_setup() {
	display[0].initR(INITR_MINI160x80_ST7735S);
	display[0].useFrameBuffer(true);
	display[0].fillScreen(ST7735_RED);
	display[0].updateScreen();

	display[1].initR(INITR_MINI160x80_ST7735S);
	display[1].useFrameBuffer(true);
	display[1].fillScreen(ST7735_GREEN);
	display[1].updateScreen();
}

void draw_meter(size_t d, int16_t x, int16_t y, int16_t w, int16_t h, float level) {
	int16_t red_thresh    = h / 4 * 3;
	int16_t yellow_thresh = h / 2;
	int16_t value         = (int16_t)(level * (float)h);

	// display.drawLine(x - 1, y, x - 1, y + h, ST7735_CYAN);

	// Green section
	int gfill = max(min(value, yellow_thresh), 0);
	display[d].fillRect(x, y + h - gfill, w, gfill, ST7735_GREEN);
	display[d].fillRect(x, y + h - yellow_thresh, w, yellow_thresh - gfill, RGB(0, 100, 0));

	// Yellow section
	int yfill = max(min(value, red_thresh), yellow_thresh) - yellow_thresh;
	display[d].fillRect(x, y + h - yellow_thresh - yfill, w, yfill, ST7735_YELLOW);
	display[d].fillRect(x, y + h - red_thresh, w, red_thresh - yellow_thresh - yfill, RGB(100, 100, 0));

	// Red section
	int rfill = max(min(value, h), red_thresh) - red_thresh;
	display[d].fillRect(x, y + h - red_thresh - rfill, w, rfill, ST7735_RED);
	display[d].fillRect(x, y, w, h - red_thresh - rfill, RGB(100, 0, 0));
}

void draw_channel(float rms, int id, const ChanInfo& channel_info, bool state) {
	size_t d = channel_info.display;
	uint16_t w = channel_info.link == 0 ? 12 : 24;
	if (channel_info.link != 2) {
		display[d].fillRoundRect(4 + channel_info.x, channel_info.y + (SCREEN_HEIGHT / 2) - 11, w, 10, 1, state ? channel_info.color_alt : channel_info.color);

		display[d].drawString(channel_info.label, 5 + channel_info.x, channel_info.y + (SCREEN_HEIGHT / 2) - 9);
	}
	draw_meter(d, 6 + channel_info.x, channel_info.y + 1, 10, (SCREEN_HEIGHT / 2) - 13, out_level_to_scale(rms));
}

void display_update_vu(float levels_rms[CHANNELS + BUSES], bool state[CHANNELS+BUSES]) {
	for (size_t d=0;d<2;d++)
	{
		display[d].fillScreen(RGB(0, 0, 0));
		display[d].setTextSize(1);
		display[d].setTextColor(RGB(0, 0, 0));
	}

	for (int i = 0; i < CHANNELS + BUSES; i++) {
		draw_channel(levels_rms[i], i, channel_info(i), state[i]);
	}
}

void display_update_screen() { display[0].updateScreen(); display[1].updateScreen(); }
