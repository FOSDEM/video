#pragma once
#include "filter_biquad.h"

#define SCALE (1073741824)

#define FILTER_ALLPASS (0)
#define FILTER_LOWPASS (1)
#define FILTER_HIGHPASS (2)

struct Band
{
    float gain;
    float frequency;
    float q;
    uint8_t type;
    bool enabled;
    int coef[5];
};

class Filter
{
public:
    Filter(AudioFilterBiquad* biquad)
    {
        this->biquad = biquad;
        this->SetAllpass(0);
        this->SetAllpass(1);
        this->SetAllpass(2);
        this->SetAllpass(3);
        dirty = false;
    }

    void Apply();

    // Band functions from the filter_biquad.h file in the audio lib, but
    // with re-ordering for gain

    void SetLowpass(uint8_t band, float frequency, float q = 0.7071f)
    {
        double w0 = frequency * (2.0f * 3.141592654f / AUDIO_SAMPLE_RATE_EXACT);
        double sinW0 = sin(w0);
        double alpha = sinW0 / ((double)q * 2.0);
        double cosW0 = cos(w0);
        double scale = (double)SCALE / (1.0 + alpha);
        _band[band].coef[0] = ((1.0 - cosW0) / 2.0) * scale;
        _band[band].coef[1] = (1.0 - cosW0) * scale;
        _band[band].coef[2] = _band[band].coef[0];
        _band[band].coef[3] = (-2.0 * cosW0) * scale;
        _band[band].coef[4] = (1.0 - alpha) * scale;

        _band[band].type = FILTER_LOWPASS;
        _band[band].gain = 0.0f;
        _band[band].frequency = frequency;
        _band[band].q = q;
        Apply();
    }

    void SetHighpass(uint8_t band, float frequency, float q = 0.7071) {
        double w0 = frequency * (2.0f * 3.141592654f / AUDIO_SAMPLE_RATE_EXACT);
        double sinW0 = sin(w0);
        double alpha = sinW0 / ((double)q * 2.0);
        double cosW0 = cos(w0);
        double scale = 1073741824.0 / (1.0 + alpha);
        _band[band].coef[0] = ((1.0 + cosW0) / 2.0) * scale;
        _band[band].coef[1] = -(1.0 + cosW0) * scale;
        _band[band].coef[2] = _band[band].coef[0];
        _band[band].coef[3] = (-2.0 * cosW0) * scale;
        _band[band].coef[4] = (1.0 - alpha) * scale;

        _band[band].type = FILTER_HIGHPASS;
        _band[band].gain = 0.0f;
        _band[band].frequency = frequency;
        _band[band].q = q;

        Apply();
    }

    void SetAllpass(uint8_t band)
    {
        _band[band].coef[0] = SCALE;
        _band[band].coef[1] = 0;
        _band[band].coef[2] = 0;
        _band[band].coef[3] = 0;
        _band[band].coef[4] = 0;

        _band[band].type = FILTER_ALLPASS;
        _band[band].gain = 1.0f;
        _band[band].frequency = 0.0;
        _band[band].q = 0.0;

        Apply();
    }

    Band _band[4];
    bool dirty;

private:
    AudioFilterBiquad* biquad;
};
