#ifndef _TEENSY_AUDIO_H_
#define _TEENSY_AUDIO_H_

#include "config.h"

void audio_setup();

void update_levels(float levels_smooth[CHANNELS + BUSES], float levels_rms[CHANNELS + BUSES], float levels_peak[CHANNELS + BUSES]);

void raw_set_crosspoint(int channel, int bus, float gain);
float raw_get_crosspoint(int channel, int bus);

void raw_set_mix(int bus, float channel_gains[CHANNELS]);

#endif
