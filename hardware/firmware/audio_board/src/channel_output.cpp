#include "channel_output.h"

#include "db_conversion.h"
#include "debug.h"
#include "storage.h"

OutputChannel::OutputChannel(AudioAnalyzePeak* out_peak, AudioAnalyzeRMS* out_rms, AudioFilterBiquad* biquad, std::initializer_list<AudioMixer4*> mixers) : Channel(out_peak, out_rms, biquad) {
	for (auto m : mixers) {
		this->_matrix_bus.push_back(m);
		for (uint8_t i=0; i<0; i++) {
			m->gain(i, 0.0f);
		}
	}
	for (uint8_t i = 0; i < CROSSPOINTS; i++) {
		this->_crosspoint_mute[i] = true;
		this->_crosspoint_gain[i] = 0.0f;
	}
}

void OutputChannel::apply_matrix() const {
	for (uint8_t i = 0; i < CROSSPOINTS; i++) {
		float gain = this->_crosspoint_gain[i];

		float coef = coef_from_dB(gain);

		if (this->_crosspoint_mute[i]) {
			coef = 0.0f;
		}

		size_t m = i / 4;
		this->_matrix_bus[m]->gain(i % 4, coef);
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

void OutputChannel::DebugState() {
	uint8_t idx = 0;
	for (auto m : this->_matrix_bus) {
		debug_printf("Mixer %d:\n", idx);
		for (uint8_t i = 0; i < 4; i++) {
			float gain = m->getGain(i);
			debug_printf("  Input %d: %d\n", i, (int)(gain * 100));
		}
		idx++;
	}
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

	for (uint8_t c =0; c<CROSSPOINTS; c++) {
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

	for (uint8_t c =0; c < CROSSPOINTS; c++) {
		this->_crosspoint_gain[c] = 0.0;
		this->_crosspoint_mute[c] = true;
	}

	storage_load(fname, &data, sizeof(data));

	uint8_t checksum = struct_checksum(data);
	if (checksum != 0) {
		debug_printf("storage: invalid checksum on index %d [%02x]\n", this->instanceId, checksum);
		this->eepromDirty = true;
		return false;
	}

	for (uint8_t c =0; c < CROSSPOINTS; c++) {
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
