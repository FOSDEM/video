#pragma once
#include "channel.h"

struct __attribute__((packed)) EepromInput {
		float        gain;
		bool         phantom;
		EepromBiquad eq[4];
		uint8_t      checksum;
};

class InputChannel : public Channel {
	public:
		InputChannel(AudioAnalyzePeak* in_peak, AudioAnalyzeRMS* in_rms, AudioFilterBiquad* biquad);

		void SetGain(float gainDb);

		float GetGain() const;

		bool EepromSave();
		bool EepromLoad();

		void HandlePhantom(uint8_t arg, void (*handler)(uint8_t, bool));

		void HandleAnalogGain(int arg, float (*set_gain)(int, float));

		[[nodiscard]] bool IsPhantomEnabled() const;

		void SetPhantom(bool enable);

	private:
		bool    has_phantom;
		bool    phantom_enabled;
		uint8_t phantom_arg;
		void (*phantom_handler)(uint8_t, bool);

		bool has_analog_gain;
		int  analog_gain_arg;
		float (*adc_handler)(int, float);

		float digital_gain = 0.0f;
		float analog_gain  = 0.0f;
};
