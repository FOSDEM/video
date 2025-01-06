#ifndef _MIXER_DISPLAY_H_
#define _MIXER_DISPLAY_H_

#include "config.h"

#include "channels.h"

#include <ST7735_t3.h>

#define SCREEN_WIDTH 80   // OLED display width, in pixels
#define SCREEN_HEIGHT 160 // OLED display height, in pixels

void display_setup();

void display_update_vu(float levels_rms[CHANNELS + BUSES],
                       ChanInfo channel_info[CHANNELS + BUSES]);

void display_update_screen();

#endif
