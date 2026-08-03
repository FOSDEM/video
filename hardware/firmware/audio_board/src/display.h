#pragma once

#include "config.h"

#include "channel_info.h"

#include <ST7735_t3.h>

#define SCREEN_WIDTH  80  // OLED display width, in pixels
#define SCREEN_HEIGHT 160 // OLED display height, in pixels

void display_setup();

void display_update_vu(float levels_rms[CHANNELS + BUSES], bool state[CHANNELS + BUSES]);

void display_update_screen();
