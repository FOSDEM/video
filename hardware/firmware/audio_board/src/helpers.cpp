#include <math.h>

float rmsToDb(float rms_in) {
    // 0.64 == +4 dBu
    // 0.57 == +2 dBu
    // 0.47 = 0 dBu
    // 0.37 = -2 dBu
    // 0.3  = -4 dBu
    // 0.24 = -6 dBu
    // 0.19 = -8 dBu
    // 0.15 = -10 dBu
    // 0.12 = -12 dBu
    // 0.09 = -14 dBu
    // 0.07 = -16 dBu
    // 0.06 = -18 dBu
    // 0.048 = -20 dBu
    // 0.037 = -22 dBu

    float Vrms = rms_in * 1.648f;
    float dB = 20.0f * log10f(Vrms / 0.775f);
    return dB;
}

float DbtoLevel(float db) {
    float e = 2.71828f;
    return 0.79306f * powf(e, db * 0.0527087f);
}
