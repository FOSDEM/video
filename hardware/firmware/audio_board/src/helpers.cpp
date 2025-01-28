#include <math.h>

#define GAIN (1.2258f)

float db(float vrms) { return 20.0f * log10f(vrms / 0.775f); }

// the inverse eof db(vrms)
float vrms(float db) { return powf(10, (db / 20.0f)) * 0.775f; }

float rmsToDb(float in_v) {
    // 1.002 == +4 dBu
    // 0.796 == +2 dBu
    // 0.632 == +0 dBu
    // 0.502 == -2 dBu
    // 0.399 == -4 dBu
    // 0.317 == -6 dBu
    // 0.252 == -8 dBu
    // 0.200 == -10 dBu
    // 0.159 == -12 dBu
    // 0.126 == -14 dBu
    // 0.100 == -16 dBu
    // 0.080 == -18 dBu
    // 0.063 == -20 dBu
    // 0.050 == -22 dBu
    return db(in_v * GAIN);
}

// the inverse of rmsToDb
float dbToRms(float db) { return vrms(db) / GAIN; }

float DbtoLevel(float db) {
    float e = 2.71828f;
    return 0.79306f * powf(e, db * 0.0527087f);
}
