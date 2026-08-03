#include "channel_strip.h"

#include "db_conversion.h"
#include "debug.h"
#include "storage.h"

int Channel::instances = 0;

Channel::Channel(AudioAnalyzePeak* in_peak, AudioAnalyzeRMS* in_rms, AudioFilterBiquad* biquad) : filter(biquad) {
	this->_peak             = in_peak;
	this->_rms              = in_rms;
	this->_biquad           = biquad;
	this->_level_multiplier = 1.0f;

	this->level_peak   = 0.0f;
	this->level_rms    = 0.0f;
	this->level_smooth = 0.0f;

	this->instanceId = this->instances++;
}

void Channel::update() {
	float rms          = this->_rms->read() * this->_level_multiplier;
	this->level_peak   = this->_peak->read() * this->_level_multiplier;
	this->level_smooth = (this->level_smooth * 9 + rms) / 10;

	if (rms < this->level_rms) {
		this->level_rms *= 0.97;
	} else {
		this->level_rms = rms;
	}
}

void Channel::SetLevelCalibration(float multiplier) {
	this->_level_multiplier = multiplier;
}

InputChannel::InputChannel(AudioAnalyzePeak* in_peak, AudioAnalyzeRMS* in_rms, AudioFilterBiquad* biquad) : Channel(in_peak, in_rms, biquad) {}

void InputChannel::SetGain(float gainDb) {
	float dgain = gainDb;
	if (this->has_analog_gain) {
		this->analog_gain = this->adc_handler(this->analog_gain_arg, dgain);
		dgain -= this->analog_gain;
	}
	this->digital_gain = dgain;
	this->eepromDirty  = true;
}

float InputChannel::GetGain() const {
	return this->digital_gain + this->analog_gain;
}

void InputChannel::SetPhantomPowerPin(uint8_t pin) {
	pinMode(pin, OUTPUT);
	this->has_phantom = true;
	this->phantom_pin = pin;
}

void InputChannel::HandleAnalogGain(int arg, float (*set_gain)(int, float)) {
	this->has_analog_gain = true;
	this->analog_gain_arg = arg;
	this->adc_handler     = set_gain;
}

bool InputChannel::IsPhantomEnabled() const {
	return this->phantom_enabled;
}

void InputChannel::SetPhantom(bool enable) {
	if (!this->has_phantom) {
		return;
	}
	digitalWrite(this->phantom_pin, enable);
	this->phantom_enabled = enable;
	this->eepromDirty     = true;
	this->EepromSave();
}

bool InputChannel::EepromSave() {
	if (this->filter.dirty) {
		this->eepromDirty = true;
	}
	if (!this->eepromDirty) {
		return false;
	}

	char fname[10];
	sprintf(fname, "input.%d", this->instanceId);

	EepromInput data = {
		.gain     = this->analog_gain + this->digital_gain,
		.phantom  = this->phantom_enabled,
		.checksum = 0,
	};

	for (uint8_t i = 0; i < 4; i++) {
		data.eq[i].type      = this->filter._band[i].type;
		data.eq[i].frequency = this->filter._band[i].frequency;
		data.eq[i].gain      = this->filter._band[i].gain;
		data.eq[i].q         = this->filter._band[i].q;
	}

	auto checksum = struct_checksum(data);
	data.checksum = checksum;

	if (!storage_save(fname, &data, sizeof(data))) {
		debug_printf("storage: issue writing\n");
		return false;
	}

	this->eepromDirty  = false;
	this->filter.dirty = false;
	return true;
}

bool InputChannel::EepromLoad() {
	char fname[10];
	sprintf(fname, "input.%d", this->instanceId);
	EepromInput data = {0};

	storage_load(fname, &data, sizeof(data));

	uint8_t checksum = struct_checksum(data);
	if (checksum != 0) {
		debug_printf("storage: invalid checksum on index %d [%02x]\n", this->instanceId, checksum);
		this->eepromDirty = true;
		return false;
	}

	debug_printf("input: Restored %s\n", fname);

	this->SetGain(data.gain);
	this->SetPhantom(data.phantom);

	for (uint8_t i = 0; i < 4; i++) {
		if (!this->filter.SetFilter(i, data.eq[i].type, data.eq[i].frequency, data.eq[i].gain, data.eq[i].q)) {
			debug_printf("input: Unknown filter %d\n", data.eq[i].type);
		}
	}

	return true;
}

OutputChannel::OutputChannel(AudioAnalyzePeak* out_peak, AudioAnalyzeRMS* out_rms, AudioFilterBiquad* biquad, std::initializer_list<AudioMixer4*> mixers) : Channel(out_peak, out_rms, biquad) {
	for (auto m : mixers) {
		this->_matrix_bus.push_back(m);
		for (uint8_t i=0; i<0; i++) {
			m->gain(i, 0.0f);
		}
	}
}

void OutputChannel::apply_matrix() const {
	for (uint8_t i = 0; i < CHANNELS; i++) {
		float gain = this->_crosspoint_gain[i];

		if (this->_crosspoint_mute[i]) {
			gain = 0.0f;
		}

		size_t m = i / 4;
		this->_matrix_bus[m]->gain(i % 4, coef_from_dB(gain));
	}
}

void OutputChannel::SetCrosspointLevel(int input_index, float gain) {
	this->_crosspoint_gain[input_index] = gain;
	this->apply_matrix();
}

float OutputChannel::GetCrosspointLevel(int input_index) {
	return this->_crosspoint_gain[input_index];
}

void OutputChannel::SetCrosspointMute(int input_index, bool mute) {
	this->_crosspoint_mute[input_index] = mute;
	this->apply_matrix();
}

bool OutputChannel::GetCrosspointMute(int input_index) {
	return this->_crosspoint_mute[input_index];
}

bool OutputChannel::EepromSave() {
	if (this->filter.dirty) {
		this->eepromDirty = true;
	}
	if (!this->eepromDirty) {
		return false;
	}

	char fname[10];
	sprintf(fname, "output.%d", this->instanceId);

	EepromOutput data = {
		.checksum = 0,
	};

	for (uint8_t c =0; c<CHANNELS; c++) {
		data.matrix_gain[c] = this->_crosspoint_gain[c];
		data.matrix_mute[c] = this->_crosspoint_mute[c];
	}

	for (uint8_t i = 0; i < 4; i++) {
		data.eq[i].type      = this->filter._band[i].type;
		data.eq[i].frequency = this->filter._band[i].frequency;
		data.eq[i].gain      = this->filter._band[i].gain;
		data.eq[i].q         = this->filter._band[i].q;
	}

	auto checksum = struct_checksum(data);
	data.checksum = checksum;

	if (!storage_save(fname, &data, sizeof(data))) {
		debug_printf("storage: issue writing\n");
		return false;
	}

	this->eepromDirty  = false;
	this->filter.dirty = false;
	return true;
}

bool OutputChannel::EepromLoad() {
	char fname[10];
	sprintf(fname, "output.%d", this->instanceId);
	EepromOutput data = {0};

	for (uint8_t c =0; c<CHANNELS; c++) {
		this->_crosspoint_gain[c] = 1.0;
		this->_crosspoint_mute[c] = c > (CHANNELS-3);
	}

	storage_load(fname, &data, sizeof(data));

	uint8_t checksum = struct_checksum(data);
	if (checksum != 0) {
		debug_printf("storage: invalid checksum on index %d [%02x]\n", this->instanceId, checksum);
		this->eepromDirty = true;
		return false;
	}

	for (uint8_t c =0; c<CHANNELS; c++) {
		this->_crosspoint_gain[c] = data.matrix_gain[c];
		this->_crosspoint_mute[c] = data.matrix_mute[c];
	}

	debug_printf("output: Restored %s\n", fname);

	this->apply_matrix();

	for (uint8_t i = 0; i < 4; i++) {
		if (!this->filter.SetFilter(i, data.eq[i].type, data.eq[i].frequency, data.eq[i].gain, data.eq[i].q)) {
			debug_printf("input: Unknown filter %d\n", data.eq[i].type);
		}
	}

	return true;
}
