#pragma once
#include "channel.h"
#include "channel_info.h"
#include "mixer.h"

#include <initializer_list>
#include <vector>

struct __attribute__((packed)) EepromOutput {
	EepromBiquad eq[4];
	float        matrix_gain[CHANNELS];
	bool         matrix_mute[CHANNELS];
	uint8_t      checksum;
};

class OutputChannel : public Channel {
	public:
		OutputChannel(AudioAnalyzePeak* out_peak, AudioAnalyzeRMS* out_rms, AudioFilterBiquad* biquad, std::initializer_list<AudioMixer4*> mixers);

		bool EepromSave();
		bool EepromLoad();

		void SetCrosspointLevel(int input_index, float gain);

		float GetCrosspointLevel(int input_index);

		void SetCrosspointMute(int input_index, bool mute);

		bool GetCrosspointMute(int input_index);

	private:
		void                      apply_matrix() const;
		std::vector<AudioMixer4*> _matrix_bus;
		float                     _crosspoint_gain[CHANNELS];
		bool                      _crosspoint_mute[CHANNELS];
};
