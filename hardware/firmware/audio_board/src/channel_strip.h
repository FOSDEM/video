#pragma once
#include "analyze_peak.h"
#include "analyze_rms.h"
#include "channels.h"
#include "debug.h"
#include "filter.h"
#include "filter_biquad.h"
#include "mixer.h"

struct __attribute__((packed)) EepromBiquad {
		uint8_t type;
		float   gain;
		float   frequency;
		float   q;
};

struct __attribute__((packed)) EepromInput {
		float        gain;
		bool         phantom;
		EepromBiquad eq[4];
		uint8_t      checksum;
};

struct __attribute__((packed)) EepromOutput {
	EepromBiquad eq[4];
	float        matrix_gain[CHANNELS];
	bool         matrix_mute[CHANNELS];
	uint8_t      checksum;
};

class Channel {
	public:
		float level_peak;
		float level_rms;
		float level_smooth;

		Filter filter;

		Channel(AudioAnalyzePeak* in_peak, AudioAnalyzeRMS* in_rms, AudioFilterBiquad* biquad);

		void update();

		void SetLevelCalibration(float multiplier);

	protected:
		float _level_multiplier;

		static int instances;
		int        instanceId;
		bool       eepromDirty;

	private:
		AudioAnalyzePeak*  _peak;
		AudioAnalyzeRMS*   _rms;
		AudioFilterBiquad* _biquad;
};

class InputChannel : public Channel {
	public:
		InputChannel(AudioAnalyzePeak* in_peak, AudioAnalyzeRMS* in_rms, AudioFilterBiquad* biquad);

		void SetGain(float gainDb);

		float GetGain() const;

		bool EepromSave();
		bool EepromLoad();

		void SetPhantomPowerPin(uint8_t pin);

		void HandleAnalogGain(int arg, float (*set_gain)(int, float));

		[[nodiscard]] bool IsPhantomEnabled() const;

		void SetPhantom(bool enable);

	private:
		bool    has_phantom;
		bool    phantom_enabled;
		uint8_t phantom_pin = 0;

		bool has_analog_gain;
		int  analog_gain_arg;
		float (*adc_handler)(int, float);

		float digital_gain = 0.0f;
		float analog_gain  = 0.0f;
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
