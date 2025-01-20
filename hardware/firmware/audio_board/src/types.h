#ifndef _MIXER_TYPES_H_
#define _MIXER_TYPES_H_

#include "config.h"
#include <stdint.h>

typedef struct {
    float gains[CHANNELS][BUSES];

    uint64_t mutes;

    float channel_multipliers[CHANNELS];
    float bus_multipliers[BUSES];
} AudioState;

typedef struct {
    float smooth[CHANNELS + BUSES];
    float peak[CHANNELS + BUSES];
    float rms[CHANNELS + BUSES];
} Levels;

#endif
