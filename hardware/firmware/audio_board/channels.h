#ifndef _CHANNELS_H_
#define _CHANNELS_H_

#include <stdint.h>

typedef struct ChanInfo {
                uint16_t color;
                char label[4];
                char desc[16];
                uint8_t link;
} ChanInfo;

#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))

#define CHAN_WHITE RGB(255, 255, 255)
#define CHAN_YELLOW RGB(255, 255, 80)
#define CHAN_MAGENTA RGB(255, 140, 255)
#define CHAN_GREEN RGB(100, 255, 100)

#endif
