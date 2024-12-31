#include "display.h"
#include "channels.h"
#include "helpers.h"
#include "channels.h"

#include <ST7735_t3.h>

void display_setup(ST7735_t3 *display) {
	display->initR(INITR_MINI160x80_ST7735S);
	display->useFrameBuffer(true);
	display->fillScreen(ST7735_RED);
	display->updateScreen();
}

void draw_meter(ST7735_t3 *display, int16_t x, int16_t y, int16_t w, int16_t h, float level) {
	int16_t red_thresh = h / 4 * 3;
	int16_t yellow_thresh = h / 2;
	int16_t value = (int16_t) (level * (float) h);

	// display->drawLine(x - 1, y, x - 1, y + h, ST7735_CYAN);

	// Green section
	int gfill = max(min(value, yellow_thresh), 0);
	display->fillRect(x, y + h - gfill, w, gfill, ST7735_GREEN);
	display->fillRect(x, y + h - yellow_thresh, w, yellow_thresh - gfill, RGB(0, 100, 0));

	// Yellow section
	int yfill = max(min(value, red_thresh), yellow_thresh) - yellow_thresh;
	display->fillRect(x, y + h - yellow_thresh - yfill, w, yfill, ST7735_YELLOW);
	display->fillRect(x, y + h - red_thresh, w, red_thresh - yellow_thresh - yfill, RGB(100, 100, 0));

	// Red section
	int rfill = max(min(value, h), red_thresh) - red_thresh;
	display->fillRect(x, y + h - red_thresh - rfill, w, rfill, ST7735_RED);
	display->fillRect(x, y, w, h - red_thresh - rfill, RGB(100, 0, 0));
}

void draw_channel(ST7735_t3 *display, int rms, int id, ChanInfo *channel_info) {
	uint16_t offset = 0;
	if (id < CHANNELS) {
	} else {
		// Outputs
		offset += (SCREEN_HEIGHT / 2);
	}
	uint16_t w = channel_info->link == 0 ? 12 : 24;
	if (channel_info->link != 2) {
		display->fillRoundRect(4 + ((id % 6) * 12), offset + (SCREEN_HEIGHT / 2) - 11, w, 10, 1,
				channel_info->color);

		display->drawString(channel_info->label, 5 + ((id % 6) * 12), offset + (SCREEN_HEIGHT / 2) - 9);
	}
	draw_meter(display, 6 + (12 * (id % 6)), offset + 1, 10, (SCREEN_HEIGHT / 2) - 13, DbtoLevel(rmsToDb(rms)));
}

void update_display(ST7735_t3 *display, float levels_rms[CHANNELS + BUSES], ChanInfo channel_info[CHANNELS + BUSES]) {
                display->fillScreen(RGB(0, 0, 0));

                display->setTextSize(1);
                display->setTextColor(RGB(0, 0, 0));

                for (int i = 0; i < 12; i++) {
                        draw_channel(display, levels_rms[i], i, &channel_info[i]);
		}
}
