#include "channel_input.h"

#include "debug.h"
#include "storage.h"

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

void InputChannel::HandlePhantom(uint8_t arg, void (*handler)(uint8_t, bool)) {
	this->has_phantom     = true;
	this->phantom_arg     = arg;
	this->phantom_handler = handler;
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
	this->phantom_handler(this->phantom_arg, enable);
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
