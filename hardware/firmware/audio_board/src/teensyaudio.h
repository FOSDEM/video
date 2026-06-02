#ifndef _TEENSY_AUDIO_H_
#define _TEENSY_AUDIO_H_

#include "config.h"

#include "types.h"

#include <stdbool.h>

void audio_setup();
void audio_load_state();

void update_levels(float levels_smooth[CHANNELS + BUSES], float levels_rms[CHANNELS + BUSES], float levels_peak[CHANNELS + BUSES]);

bool is_muted(uint8_t channel, uint8_t bus);
void mute(uint8_t channel, uint8_t bus);
void unmute(uint8_t channel, uint8_t bus);

void  set_volume_dB(uint8_t channel, uint8_t bus, float vol_dB);
float get_volume_dB(uint8_t channel, uint8_t bus);

float get_bus_volume_dB(uint8_t bus);
void  set_bus_volume_dB(uint8_t bus, float vol_dB);

float get_channel_input_gain_dB(uint8_t channel);
void  set_channel_input_gain_dB(uint8_t channel, float gain);

void    audio_update_levels(Levels& levels);
Levels& audio_get_levels();

void audio_reset_default_state();

bool is_phantom_on(uint8_t channel);
void set_phantom_on(uint8_t channel);
void set_phantom_off(uint8_t channel);

#ifdef USE_EEPROM
uint8_t audio_eeprom_save_all();
#endif

#endif
