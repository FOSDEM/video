#include <Audio.h>
#include <Wire.h>

#include "config.h"

#include "teensyaudio.h"

#ifdef USE_EEPROM

#include "storage.h"
#define STATE_EEPROM_OFFSET 0x0

#endif

#include "teensyaudio_defaults.cpp"
#include "teensyaudio_generated.cpp"

AudioControlSGTL5000 sgtl5000_1;
AudioControlSGTL5000 sgtl5000_2;

AudioMixer4 *matrix[6][2] = {
    {&mixer1, &mixer2},   {&mixer4, &mixer5},   {&mixer7, &mixer8},
    {&mixer10, &mixer11}, {&mixer13, &mixer14}, {&mixer16, &mixer17},
};

AudioAnalyzeRMS *ent_rms[12] = {
    &rms1, &rms2, &rms3, &rms4,  &rms5,  &rms6,
    &rms7, &rms8, &rms9, &rms10, &rms11, &rms12,
};

AudioAnalyzePeak *ent_peak[12] = {
    &peak1, &peak2, &peak3, &peak4,  &peak5,  &peak6,
    &peak7, &peak8, &peak9, &peak10, &peak11, &peak12,
};

Levels levels;
AudioState state;

void audio_setup() {
    AudioMemory(64);

    uint8_t adcGain = 0;

    sgtl5000_1.enable();
    sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
    sgtl5000_1.volume(0.5);
    sgtl5000_1.lineOutLevel(13);
    sgtl5000_1.lineInLevel(adcGain, adcGain);
    sgtl5000_1.adcHighPassFilterDisable();
    sgtl5000_1.muteHeadphone();

    sgtl5000_2.setWire(1);

    sgtl5000_2.enable();
    sgtl5000_2.inputSelect(AUDIO_INPUT_LINEIN);
    sgtl5000_2.volume(0.5);
    sgtl5000_2.lineOutLevel(13);
    sgtl5000_2.lineInLevel(adcGain, adcGain);
    sgtl5000_2.adcHighPassFilterDisable();
}

void audio_update_levels(Levels &levels) {
    float temp;
    if (rms1.available()) {
        for (int i = 0; i < CHANNELS + BUSES; i++) {
            temp = ent_rms[i]->read();
            levels.smooth[i] = ((levels.smooth[i] * 9) + temp) / 10;
            temp = levels.smooth[i];
            // VU meter drains slowly after a peak
            if (temp < levels.rms[i]) {
                levels.rms[i] *= 0.97;
            } else {
                levels.rms[i] = temp;
            }
            levels.peak[i] = ent_peak[i]->read();
        }
    }
}

Levels &audio_get_levels() { return levels; }

void raw_set_crosspoint(int channel, int bus, float gain) {
    matrix[bus][channel / 4]->gain(channel % 4, gain);
}

float raw_get_crosspoint(int channel, int bus) {
    return matrix[bus][channel / 4]->getGain(channel % 4);
}

void raw_set_mix(int bus, float in1, float in2, float in3, float in4, float in5,
                 float in6) {
    matrix[bus][0]->gain(0, in1);
    matrix[bus][0]->gain(1, in2);
    matrix[bus][0]->gain(2, in3);
    matrix[bus][0]->gain(3, in4);
    matrix[bus][1]->gain(0, in5);
    matrix[bus][1]->gain(1, in6);
    matrix[bus][1]->gain(2, 0.0f);
    matrix[bus][1]->gain(3, 0.0f);
}

uint64_t mute_mask(uint64_t channel, uint64_t bus) {
    return (uint64_t)1 << (uint64_t)((channel * CHANNELS) + bus);
}

bool is_muted(int channel, int bus) {
    return !!(state.mutes & mute_mask(channel, bus));
}

float calc_real_gain(int channel, int bus, int gain) {
    return gain * !is_muted(channel, bus) * state.bus_multipliers[bus] *
           state.channel_multipliers[channel];
}

// checking if muted
void set_gain(int channel, int bus, int gain) {
    state.gains[channel][bus] = gain;

    raw_set_crosspoint(channel, bus, calc_real_gain(channel, bus, gain));
}

float get_gain(int channel, int bus) { return state.gains[channel][bus]; }

// FIXME: put `unmute` and `unmute` in the same function
void mute(int channel, int bus) {
    state.mutes |= mute_mask(channel, bus); // side effect
    set_gain(channel, bus, state.gains[channel][bus]);
}

void unmute(int channel, int bus) {
    state.mutes &= ~mute_mask(channel, bus); // side effect
    set_gain(channel, bus, state.gains[channel][bus]);
}

void set_bus_multiplier(int bus, float multiplier) {
    state.bus_multipliers[bus] = multiplier;
    for (int channel = 0; channel < CHANNELS; ++channel)
        set_gain(channel, bus, state.gains[channel][bus]);
}

float get_bus_multiplier(int bus) { return state.bus_multipliers[bus]; }

void set_channel_multiplier(int channel, float multiplier) {
    state.channel_multipliers[channel] = multiplier;
    for (int bus = 0; bus < BUSES; ++bus)
        set_gain(channel, bus, state.gains[channel][bus]);
}

float get_channel_multiplier(int channel) {
    return state.channel_multipliers[channel];
}

void reset_gains() { memcpy(state.gains, default_gains, sizeof(state.gains)); }

void reset_mutes() {
    memcpy(&state.mutes, &default_mutes, sizeof(state.mutes));
}

void reset_bus_multipliers() {
    memcpy(state.bus_multipliers, default_bus_multipliers,
           BUSES * sizeof(float));
}
void reset_channel_multipliers() {
    memcpy(state.channel_multipliers, default_channel_multipliers,
           CHANNELS * sizeof(float));
}

void apply_all() {
    int i, j;
    for (i = 0; i < CHANNELS; ++i)
        for (j = 0; j < BUSES; ++j)
            set_gain(i, j, state.gains[i][j]);
}

void audio_reset_default_state() {
    reset_gains();
    reset_mutes();
    reset_bus_multipliers();
    reset_channel_multipliers();
}

bool gains_ok() {
    int i, j;
    for (i = 0; i < CHANNELS; ++i)
        for (j = 0; j < BUSES; ++j)
            if (isnan(state.gains[i][j]))
                return false;

    return true;
}

bool bus_multipliers_ok() {
    int i;
    for (i = 0; i < BUSES; ++i)
        if (isnan(state.bus_multipliers[i]))
            return false;

    return true;
}

bool channel_multipliers_ok() {
    int i;
    for (i = 0; i < CHANNELS; ++i)
        if (isnan(state.channel_multipliers[i]))
            return false;

    return true;
}

#ifdef USE_EEPROM
uint8_t audio_eeprom_save_all() {
    return eeprom_save_all(state, STATE_EEPROM_OFFSET);
}
#endif

void audio_load_state() {
#ifdef USE_EEPROM
    eeprom_load_all(state, STATE_EEPROM_OFFSET);

    if (!gains_ok() || !bus_multipliers_ok() || !channel_multipliers_ok()) {
        audio_reset_default_state();
        audio_eeprom_save_all();
    }

#else
    audio_reset_default_state();
#endif
    apply_all();
}
