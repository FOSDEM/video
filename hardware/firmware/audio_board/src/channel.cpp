#include "channel.h"

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
