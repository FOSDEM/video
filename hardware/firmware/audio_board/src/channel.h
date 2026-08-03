#pragma once
#include "analyze_peak.h"
#include "analyze_rms.h"
#include "filter.h"
#include "filter_biquad.h"

struct __attribute__((packed)) EepromBiquad {
		uint8_t type;
		float   gain;
		float   frequency;
		float   q;
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
