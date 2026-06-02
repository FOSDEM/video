#include <math.h>
#include "config.h"

float coef_to_dB(float x) {
	// convert coefficient to dB (amplitude, not power)
	return 20.0f * log10f(x);
}

float coef_from_dB(float db) {
	return powf(10, (db / 20.0f));
}

float volts_to_dBv(float v) {
	return coef_to_dB(v);
}

float volts_from_dBv(float dbv) {
	return coef_from_dB(dbv);
}

float volts_to_dBu(float v) {
	return coef_to_dB(v / 0.775f);
}

float volts_from_dBu(float dbu) {
	return 0.775 * coef_from_dB(dbu);
}

float scale_from_dB(float db) {
	// This cursed formula is used to convert a db value
	// to a 0-1 value for the purpose of displaying on a
	// vu meter. It is almost linear but compresses the
	// low end a bit so that e.g. -100db is just a bit lower than
	// -80db, but not so that they are both zero
	float e = 2.71828f;
	return 0.79306f * powf(e, db * 0.0527087f);
}

float out_level_to_dBu(float level) {
	return volts_to_dBu(level * OUT_SPAN_VOLTS);
}

float out_level_to_scale(float level) {
	return scale_from_dB(out_level_to_dBu(level));
}
