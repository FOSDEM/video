#include <Audio.h>
#include <Wire.h>

#include "config.h"
#include "db_conversion.h"
#include "teensyaudio.h"

#ifdef USE_EEPROM

#include "storage.h"
#define STATE_EEPROM_OFFSET 0x0

#endif

#include "taa3040.h"
#include "teensyaudio_defaults.cpp"
#include "teensyaudio_generated.cpp"

AudioControlTAA3040 taa3040;

AudioMixer4* matrix[8][3] = {
	{&mixer1, &mixer2, &mixer3},
	{&mixer5, &mixer6, &mixer7},
	{&mixer9, &mixer10, &mixer11},
	{&mixer13, &mixer14, &mixer15},
	{&mixer17, &mixer18, &mixer19},
	{&mixer21, &mixer22, &mixer23},
	{&mixer25, &mixer26, &mixer27},
	{&mixer29, &mixer30, &mixer31},
};

AudioAnalyzeRMS* ent_rms[16] = {
	&rms1,
	&rms2,
	&rms3,
	&rms4,
	&rms5,
	&rms6,
	&rms7,
	&rms8,
	&rms9,
	&rms10,
	&rms11,
	&rms12,
	&rms13,
	&rms14,
	&rms15,
	&rms16,
};

AudioAnalyzePeak* ent_peak[16] = {
	&peak1,
	&peak2,
	&peak3,
	&peak4,
	&peak5,
	&peak6,
	&peak7,
	&peak8,
	&peak9,
	&peak10,
	&peak11,
	&peak12,
	&peak13,
	&peak14,
	&peak15,
	&peak16,
};

Levels     levels;
AudioState state;

void audio_setup() {
	AudioMemory(80);

	Wire.begin();
	Wire1.begin();

	pinMode(PIN_PHANTOM_IN1, OUTPUT);
	pinMode(PIN_PHANTOM_IN2, OUTPUT);
	pinMode(PIN_PHANTOM_IN3, OUTPUT);

	taa3040.enable();
}

uint8_t get_channel_gain_dB_analog(uint8_t channel) {
	float gain = state.channel_input_gains_dB[channel];

	// the analog gain of the TAA3040 is between 0 and 42dB
	// and has a whole-number resolution

	if (gain < 0) {
		return 0;
	}
	if (gain > 42) {
		return 42;
	}

	return (uint8_t)(gain + 0.5);
}

float get_channel_gain_dB_digital(uint8_t channel) {
	float gain = state.channel_input_gains_dB[channel];
	return gain - (float)get_channel_gain_dB_analog(channel);
}

float level_multiplier_chan(uint8_t chan_id) {
	float db = get_channel_gain_dB_digital(chan_id);
	return coef_from_dB(db);
}

float level_multiplier_bus(uint8_t bus_id) {
	return 1;
}

float level_multiplier(uint8_t meter_id) {
	if (meter_id < CHANNELS) {
		return level_multiplier_chan(meter_id);
	} else {
		return level_multiplier_bus(meter_id - CHANNELS);
	}
}

void audio_update_levels(Levels& levels) {
	float temp;
	if (rms1.available()) {
		for (uint8_t i = 0; i < CHANNELS + BUSES; i++) {
			temp             = ent_rms[i]->read() * level_multiplier(i);
			levels.smooth[i] = ((levels.smooth[i] * 9) + temp) / 10;
			temp             = levels.smooth[i];
			// VU meter drains slowly after a peak
			if (temp < levels.rms[i]) {
				levels.rms[i] *= 0.97;
			} else {
				levels.rms[i] = temp;
			}
			levels.peak[i] = ent_peak[i]->read() * level_multiplier(i);
			levels.state[i] = false;
		}

		taa3040.getAsiStatus();

		levels.state[0] = is_phantom_on(0);
		levels.state[1] = is_phantom_on(1);
		levels.state[2] = is_phantom_on(2);

		levels.state[6] = !spdif1.isLocked();
	}
}

Levels& audio_get_levels() { return levels; }

void raw_set_crosspoint(uint8_t channel, uint8_t bus, float volume) {
	matrix[bus][channel / 4]->gain(channel % 4, volume);
}

float raw_get_crosspoint(uint8_t channel, uint8_t bus) {
	return matrix[bus][channel / 4]->getGain(channel % 4);
}

uint16_t mute_mask(uint64_t channel) {
	return (uint16_t)1 << (uint16_t)(channel);
}

void apply_phantom(uint8_t channel) {
	bool is_on = (state.phantoms & (1 << channel)) > 0;
	switch (channel) {
	case 0:
		digitalWrite(PIN_PHANTOM_IN1, is_on ? HIGH : LOW);
		break;
	case 1:
		digitalWrite(PIN_PHANTOM_IN2, is_on ? HIGH : LOW);
		break;
	case 2:
		digitalWrite(PIN_PHANTOM_IN3, is_on ? HIGH : LOW);
		break;
	}
}

bool is_phantom_on(uint8_t channel) {
	return !!(state.phantoms & (1 << channel));
}

void set_phantom_on(uint8_t channel) {
	if (channel > 2) {
		return;
	}
	state.phantoms |= (1 << channel);
	apply_phantom(channel);
}

void set_phantom_off(uint8_t channel) {
	if (channel > 2) {
		return;
	}
	state.phantoms &= ~(1 << channel);
	apply_phantom(channel);
}

bool is_muted(uint8_t channel, uint8_t bus) {
	return !!(state.mutes[bus] & mute_mask(channel));
}

float get_volume_dB(uint8_t channel, uint8_t bus) {
	return state.matrix_dB[channel][bus];
}

float get_bus_volume_dB(uint8_t bus) {
	return state.bus_volumes_dB[bus];
}

void apply_channel_input_gain(uint8_t channel) {
	set_channel_input_gain_dB(channel, state.channel_input_gains_dB[channel]);
}

float get_channel_input_gain_dB(uint8_t channel) {
	return state.channel_input_gains_dB[channel];
}

float calc_real_volume(uint8_t channel, uint8_t bus) {
	float input_volume_dB  = get_channel_gain_dB_digital(channel);
	float matrix_volume_dB = state.matrix_dB[channel][bus];
	float bus_volume_dB    = state.bus_volumes_dB[bus];

	float volume_dB = input_volume_dB + matrix_volume_dB + bus_volume_dB;
	float volume    = coef_from_dB(volume_dB);
	return volume * !is_muted(channel, bus);
}

void apply_volume(uint8_t channel, uint8_t bus) {
	raw_set_crosspoint(channel, bus, calc_real_volume(channel, bus));
}

void mute(uint8_t channel, uint8_t bus) {
	state.mutes[bus] |= mute_mask(channel);
	apply_volume(channel, bus);
}

void unmute(uint8_t channel, uint8_t bus) {
	state.mutes[bus] &= ~mute_mask(channel);
	apply_volume(channel, bus);
}

void set_volume_dB(uint8_t channel, uint8_t bus, float volume) {
	state.matrix_dB[channel][bus] = volume;
	apply_volume(channel, bus);
}

void set_bus_volume_dB(uint8_t bus, float vol) {
	state.bus_volumes_dB[bus] = vol;
	for (uint8_t channel = 0; channel < CHANNELS; ++channel) {
		apply_volume(channel, bus);
	}
}

void set_channel_input_gain_dB(uint8_t channel, float gain) {
	state.channel_input_gains_dB[channel] = gain;

	uint8_t analog_gain = get_channel_gain_dB_analog(channel);
	taa3040.gain(channel, analog_gain, IMPEDANCE_10k, 0, 0);

	for (uint8_t bus = 0; bus < BUSES; ++bus) {
		apply_volume(channel, bus);
	}
}

void reset_matrix() {
	memcpy(state.matrix_dB, default_matrix_dB, sizeof(state.matrix_dB));
}

void reset_mutes() {
	memcpy(state.mutes, &default_mutes, sizeof(state.mutes) * 2);
}

void reset_phantoms() {
	memcpy(&state.phantoms, &default_phantoms, sizeof(state.phantoms));
}

void reset_bus_volumes() {
	memcpy(state.bus_volumes_dB, default_bus_volumes_dB, BUSES * sizeof(float));
}

void reset_channel_input_gains() {
	memcpy(state.channel_input_gains_dB, default_channel_input_gains_dB, CHANNELS * sizeof(float));
}

void apply_all() {
	uint8_t i, j;
	for (i = 0; i < CHANNELS; ++i) {
		apply_channel_input_gain(i);
		apply_phantom(i);
		for (j = 0; j < BUSES; ++j) {
			apply_volume(i, j);
		}
	}
}

void audio_reset_default_state() {
	reset_matrix();
	reset_mutes();
	reset_phantoms();
	reset_bus_volumes();
	reset_channel_input_gains();
}

bool matrix_ok() {
	uint8_t i, j;
	for (i = 0; i < CHANNELS; ++i) {
		for (j = 0; j < BUSES; ++j) {
			if (isnan(state.matrix_dB[i][j])) {
				return false;
			}
		}
	}

	return true;
}

bool bus_volumes_ok() {
	uint8_t i;
	for (i = 0; i < BUSES; ++i) {
		if (isnan(state.bus_volumes_dB[i])) {
			return false;
		}
	}

	return true;
}

bool channel_input_gains_ok() {
	uint8_t i;
	for (i = 0; i < CHANNELS; ++i) {
		if (isnan(state.channel_input_gains_dB[i])) {
			return false;
		}
	}

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

	if (!matrix_ok() || !bus_volumes_ok() || !channel_input_gains_ok()) {
		audio_reset_default_state();
		audio_eeprom_save_all();
	}

#else
	audio_reset_default_state();
#endif
	apply_all();
}
