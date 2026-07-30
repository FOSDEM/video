#pragma once
#include "filter_biquad.h"

#define SCALE           (1073741824)

#define FILTER_ALLPASS  (0)
#define FILTER_LOWPASS  (1)
#define FILTER_HIGHPASS (2)
#define FILTER_LOWSHELF (3)
#define FILTER_HIGHSHELF (4)
#define FILTER_NOTCH (5)
#define FILTER_PEAK (6)

struct Band {
    float gain;
    float frequency;
    float q;
    uint8_t type;
    bool enabled;
    int coef[5];
};

class Filter {
public:
    Filter(AudioFilterBiquad* biquad) {
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

    void SetLowpass(uint8_t band, float frequency, float q = 0.7071f) {
        double cd[6];
        double w0 = frequency * (2.0f * M_PI / AUDIO_SAMPLE_RATE_EXACT);
        double s0 = sin(w0);
        double c0 = cos(w0);
        double alpha = s0 / ((double)q * 2.0);

        cd[0] = (1.0 - c0) / 2.0;
        cd[1] = 1.0 - c0;
        cd[2] = (1.0 - c0) / 2.0;
        cd[3] = 1.0 + alpha;
        cd[4] = -2.0 * c0;
        cd[5] = 1.0 - alpha;

        SetBandDouble(band, cd, FILTER_LOWPASS, 0.0f, frequency, q);
    }

    void SetHighpass(uint8_t band, float frequency, float q = 0.7071) {
        double cd[6];
        double w0 = frequency * (2.0f * M_PI / AUDIO_SAMPLE_RATE_EXACT);
        double s0 = sin(w0);
        double c0 = cos(w0);
        double alpha = s0 / ((double)q * 2.0);

        cd[0] = (1.0 + c0) / 2.0;
        cd[1] = -(1.0 + c0);
        cd[2] = (1.0 + c0) / 2.0;
        cd[3] = 1.0 + alpha;
        cd[4] = -2.0 * c0;
        cd[5] = 1.0 - alpha;

        SetBandDouble(band, cd, FILTER_HIGHPASS, 0.0f, frequency, q);
    }

    void SetAllpass(uint8_t band) {
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

    void SetLowShelf(uint8_t band, float frequency, float gain) {
        double cd[6];
        double A = pow(10.0, gain / 40.0f);
        double w0 = frequency * (2.0f * M_PI / AUDIO_SAMPLE_RATE_EXACT);
        double s0 = sin(w0);
        double c0 = cos(w0);
        double beta = sqrt(A + A);

        cd[0] = A * ((A + 1) - (A - 1) * c0 + beta * s0);
        cd[1] = 2 * A * ((A - 1) - (A + 1) * c0);
        cd[2] = A * ((A + 1) - (A - 1) * c0 - beta * s0);
        cd[3] = (A + 1) + (A - 1) * c0 + beta * s0;
        cd[4] = -2 * ((A - 1) + (A + 1) * c0);
        cd[5] = (A + 1) + (A - 1) * c0 - beta * s0;

        SetBandDouble(band, cd, FILTER_LOWSHELF, gain, frequency, 0);
    }

    void SetHighShelf(uint8_t band, float frequency, float gain) {
        double cd[6];
        double A = pow(10.0, gain / 40.0f);
        double w0 = frequency * (2.0f * M_PI / AUDIO_SAMPLE_RATE_EXACT);
        double s0 = sin(w0);
        double c0 = cos(w0);
        double beta = sqrt(A + A);

        cd[0] = A * ((A + 1) + (A - 1) * c0 + beta * s0);
        cd[1] = -2 * A * ((A - 1) + (A + 1) * c0);
        cd[2] = A * ((A + 1) + (A - 1) * c0 - beta * s0);
        cd[3] = (A + 1) - (A - 1) * c0 + beta * s0;
        cd[4] = 2 * ((A - 1) - (A + 1) * c0);
        cd[5] = (A + 1) - (A - 1) * c0 - beta * s0;

        SetBandDouble(band, cd, FILTER_HIGHSHELF, gain, frequency, 0);
    }

    void SetNotch(uint8_t band, float frequency) {
        double cd[6];
        double w0 = frequency * (2.0f * M_PI / AUDIO_SAMPLE_RATE_EXACT);
        double s0 = sin(w0);
        double c0 = cos(w0);
        double alpha = s0 / 14.0;

        cd[0] = 1.0;
        cd[1] = -2.0 * c0;
        cd[2] = 1.0;
        cd[3] = 1.0 + alpha;
        cd[4] = -2.0 * c0;
        cd[5] = 1.0 - alpha;

        SetBandDouble(band, cd, FILTER_NOTCH, 0.0f, frequency, 0);
    }

    void SetPeak(uint8_t band, float frequency, float gain, float q = 0.7071) {
        double cd[6];
        double A = pow(10.0, gain / 40.0f);
        double w0 = frequency * (2.0f * M_PI / AUDIO_SAMPLE_RATE_EXACT);
        double s0 = sin(w0);
        double c0 = cos(w0);
        double alpha = s0 / (2 * q);

        cd[0] = 1.0 + alpha * A;
        cd[1] = -2.0 * c0;
        cd[2] = 1.0 - alpha * A;
        cd[3] = 1.0 + alpha / A;
        cd[4] = -2.0 * c0;
        cd[5] = 1.0 - alpha / A;

        SetBandDouble(band, cd, FILTER_PEAK, gain, frequency, q);
    }

    bool SetFilter(uint8_t band, uint8_t shape, float f, float gain, float q) {
        switch (shape) {
        case FILTER_ALLPASS:
            SetAllpass(band);
            break;
        case FILTER_LOWPASS:
            SetLowpass(band, f, q);
            break;
        case FILTER_HIGHPASS:
            SetHighpass(band, f, q);
            break;
        case FILTER_LOWSHELF:
            SetLowShelf(band, f, gain);
            break;
        case FILTER_HIGHSHELF:
            SetHighShelf(band, f, gain);
            break;
        case FILTER_NOTCH:
            SetNotch(band, f);
            break;
        case FILTER_PEAK:
            SetPeak(band, f, gain, q);
            break;
        default:
            return false;
        }
        return true;
    }


    Band GetBand(uint8_t band) {
        return _band[band];
    }

    Band _band[4];
    bool dirty;

private:
    AudioFilterBiquad* biquad;

    void SetBandDouble(uint8_t band, double cd[6], uint8_t type, float gain, float f, float q) {
        cd[0] /= cd[3];
        cd[1] /= cd[3];
        cd[2] /= cd[3];
        cd[4] /= cd[3];
        cd[5] /= cd[3];
        _band[band].coef[0] = cd[0] * SCALE;
        _band[band].coef[1] = cd[1] * SCALE;
        _band[band].coef[2] = cd[2] * SCALE;
        _band[band].coef[3] = cd[4] * SCALE;
        _band[band].coef[4] = cd[5] * SCALE;

        _band[band].type = type;
        _band[band].gain = gain;
        _band[band].frequency = f;
        _band[band].q = q;
        Apply();
    }
};
