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

Levels     levels;
AudioState state;

float taa3040_gainhandler(int chan, float gain)
{
	// Clamp to gain range on TAA3040
	if (gain > 42)
	{
		gain = 42;
	}
	if (gain < 0)
	{
		gain = 0;
	}
	auto gainval = static_cast<uint8_t>(std::round(gain));
	taa3040.gain(chan, gainval, IMPEDANCE_10k, 0, 0);

	return static_cast<float>(gainval);
}

void audio_setup() {
	AudioMemory(80);

	Wire.begin();
	Wire1.begin();

	route_inputs[0].SetPhantomPowerPin(PIN_PHANTOM_IN1);
	route_inputs[1].SetPhantomPowerPin(PIN_PHANTOM_IN2);
	route_inputs[2].SetPhantomPowerPin(PIN_PHANTOM_IN3);

	route_inputs[0].HandleAnalogGain(0, taa3040_gainhandler);
	route_inputs[1].HandleAnalogGain(1, taa3040_gainhandler);
	route_inputs[2].HandleAnalogGain(2, taa3040_gainhandler);
	route_inputs[3].HandleAnalogGain(3, taa3040_gainhandler);

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
	if (rms1.available()) {

		for (size_t i=0;i<8; i++)
		{
			route_inputs[i].update();
			levels.smooth[i] = route_inputs[i].level_smooth;
			levels.peak[i] = route_inputs[i].level_peak;
			levels.rms[i] = route_inputs[i].level_rms;
		}
		for (size_t i=0;i<8; i++)
		{
			size_t j = i + 8;
			route_outputs[i].update();
			levels.smooth[j] = route_outputs[i].level_smooth;
			levels.peak[j] = route_outputs[i].level_peak;
			levels.rms[j] = route_outputs[i].level_rms;
		}


		taa3040.getAsiStatus();

		levels.state[0] = route_inputs[0].IsPhantomEnabled();
		levels.state[1] = route_inputs[1].IsPhantomEnabled();
		levels.state[2] = route_inputs[2].IsPhantomEnabled();

		levels.state[6] = !spdif1.isLocked();
	}
}

Levels& audio_get_levels() { return levels; }

void raw_set_crosspoint(uint8_t channel, uint8_t bus, float volume) {
	route_outputs[bus].SetCrosspointLevel(channel, volume);
}

float raw_get_crosspoint(uint8_t channel, uint8_t bus) {
	return route_outputs[bus].GetCrosspointLevel(channel);
}

bool is_phantom_on(uint8_t channel) {
	return route_inputs[channel].IsPhantomEnabled();
}

void set_phantom_on(uint8_t channel) {
	route_inputs[channel].SetPhantom(true);
}

void set_phantom_off(uint8_t channel) {
	route_inputs[channel].SetPhantom(false);
}

bool is_muted(uint8_t channel, uint8_t bus) {
	return route_outputs[bus].GetCrosspointMute(channel);
}

float get_volume_dB(uint8_t channel, uint8_t bus) {
	return route_outputs[bus].GetCrosspointLevel(channel);
}

float get_bus_volume_dB(uint8_t bus) {
	return state.bus_volumes_dB[bus];
}

void apply_channel_input_gain(uint8_t channel) {
	set_channel_input_gain_dB(channel, state.channel_input_gains_dB[channel]);
}

float get_channel_input_gain_dB(uint8_t channel) {
	return route_inputs[channel].GetGain();
}

void set_channel_input_gain_dB(uint8_t channel, float gain)
{
	route_inputs[channel].SetGain(gain);
}

void mute(uint8_t channel, uint8_t bus) {
	route_outputs[bus].SetCrosspointMute(channel, true);
}

void unmute(uint8_t channel, uint8_t bus) {
	route_outputs[bus].SetCrosspointMute(channel, false);
}

void set_volume_dB(uint8_t channel, uint8_t bus, float volume) {
	route_outputs[bus].SetCrosspointLevel(channel, volume);
}

void set_bus_volume_dB(uint8_t bus, float vol) {
	route_outputs[bus].SetGain(vol);
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

void audio_reset_default_state() {
	reset_matrix();
	reset_mutes();
	reset_phantoms();
	reset_bus_volumes();
	reset_channel_input_gains();
}

#ifdef USE_EEPROM
uint8_t audio_eeprom_save_all() {
	uint8_t saved = 0;
	for (auto& chan : route_inputs)
	{
		if (chan.EepromSave())
		{
			saved++;
		}
	}
	return saved;
}
#endif

void audio_load_state() {
	for (auto& chan : route_inputs)
	{
		chan.EepromLoad();
	}
}
