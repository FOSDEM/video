#ifndef _MIXER_DISPLAY_H_
#define _MIXER_DISPLAY_H_

#include "config.h"

#include "channels.h"

#include <ST7735_t3.h>

#define TFT_DC (12)
#define TFT_CS (10)
#define TFT_MOSI (11)
#define TFT_RST (0xFF)
#define TFT_SCK (13)

#define SCREEN_WIDTH 80 // OLED display width, in pixels
#define SCREEN_HEIGHT 160 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET (-1)

#define SLOTS (BUSES + CHANNELS)

#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))

void display_setup(ST7735_t3 *display);

void update_display(ST7735_t3 *display, float levels_rms[CHANNELS + BUSES], ChanInfo channel_info[CHANNELS + BUSES]);

#endif
