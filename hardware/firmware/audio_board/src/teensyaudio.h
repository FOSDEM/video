#ifndef _TEENSY_AUDIO_H_
#define _TEENSY_AUDIO_H_

#include "config.h"

#include <stdbool.h>

void audio_setup();
void audio_load_state();

void update_levels(float levels_smooth[CHANNELS + BUSES],
                   float levels_rms[CHANNELS + BUSES],
                   float levels_peak[CHANNELS + BUSES]);

bool is_muted(int channel, int bus);
void mute(int channel, int bus);
void unmute(int channel, int bus);

void set_gain(int channel, int bus, int gain);
float get_gain(int channel, int bus);

float get_bus_multiplier(int bus);
void set_bus_multiplier(int bus, float multiplier);

float get_channel_multiplier(int channel);
void set_channel_multiplier(int channel, float multiplier);

// void raw_set_crosspoint(int channel, int bus, float gain);
float raw_get_crosspoint(int channel, int bus);

// void raw_set_mix(int bus, float channel_gains[CHANNELS]);

typedef struct {
    float smooth[CHANNELS + BUSES];
    float peak[CHANNELS + BUSES];
    float rms[CHANNELS + BUSES];
} Levels;

void audio_update_levels(Levels &levels);
Levels &audio_get_levels();

#endif
