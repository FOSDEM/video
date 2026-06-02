#pragma once

float coef_to_dB(float x);
float coef_from_dB(float db);

float volts_to_dBv(float v);
float volts_from_dBv(float dbv);

float volts_to_dBu(float v);
float volts_from_dBu(float dbu);

float scale_from_dB(float db);

float out_level_to_dBu(float level);
float out_level_to_scale(float level);
