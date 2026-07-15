#pragma once

#include <stdint.h>

#define CHANNELS 8
#define BUSES    8

typedef struct ChanInfo {
		uint16_t color;
	    uint16_t color_alt;
		char     label[4];
		char     desc[16];
		uint8_t  link;

	    // VU drawing info
	    uint8_t   display;
	    uint8_t   x;
	    uint8_t   y;
} ChanInfo;

const ChanInfo& channel_info(uint8_t chan_id);

#define RGB(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

#define CHAN_WHITE   RGB(255, 255, 255)
#define CHAN_YELLOW  RGB(255, 255, 80)
#define CHAN_ORANGE  RGB(255, 160, 0)
#define CHAN_MAGENTA RGB(255, 140, 255)
#define CHAN_GREEN   RGB(100, 255, 100)
#define CHAN_BLUE    RGB(0, 120, 255)
#define CHAN_GREY    RGB(100, 100, 100)
