#ifndef _MIXER_TYPES_H_
#define _MIXER_TYPES_H_

#include "config.h"
#include <stdint.h>

typedef struct {
    uint8_t gain; // 0-42 -> 0-42dB gain
    bool agc_enable;
    uint8_t volume; // 0-255 digital volume (-100 - 27dB, 201=0dB)
} ADCChannelState;

typedef struct {
    // AGC_CFG0
    uint8_t agc_target; // 0-15 -> -6dB - -36dB
    uint8_t adc_maxgain; // 0-13 -> 3dB - 42dB
} ADCState;

typedef struct {
    float gains[CHANNELS][BUSES];

    uint64_t mutes;

    float channel_multipliers[CHANNELS];
    float bus_multipliers[BUSES];
    ADCState adc;
    ADCChannelState adcchan[4];
} AudioState;

typedef struct {
    float smooth[CHANNELS + BUSES];
    float peak[CHANNELS + BUSES];
    float rms[CHANNELS + BUSES];
} Levels;

#endif
