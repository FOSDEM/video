#include <Audio.h>
#include <Wire.h>

#include "config.h"

// GUItool: begin automatically generated code
AudioInputI2SQuad i2s_quad1;  // xy=336,677
AudioSynthNoisePink pink1;    // xy=338,824
AudioSynthWaveformSine sine1; // xy=340,784
AudioInputUSB usb1;           // xy=341,743
AudioAnalyzePeak peak5;       // xy=603,433
AudioAnalyzePeak peak1;       // xy=604,299
AudioAnalyzePeak peak2;       // xy=605,332
AudioAnalyzePeak peak3;       // xy=605,366
AudioAnalyzePeak peak6;       // xy=605,467
AudioAnalyzePeak peak4;       // xy=607,399
AudioAnalyzeRMS rms5;         // xy=618,1246
AudioAnalyzeRMS rms6;         // xy=620,1279
AudioAnalyzeRMS rms3;         // xy=621,1180
AudioAnalyzeRMS rms1;         // xy=622,1115
AudioAnalyzeRMS rms2;         // xy=622,1148
AudioAnalyzeRMS rms4;         // xy=622,1213
AudioMixer4 mixer1;           // xy=787,364
AudioMixer4 mixer4;           // xy=788,502
AudioMixer4 mixer2;           // xy=789,428
AudioMixer4 mixer5;           // xy=790,566
AudioMixer4 mixer7;           // xy=791,640
AudioMixer4 mixer8;           // xy=793,704
AudioMixer4 mixer10;          // xy=795,773
AudioMixer4 mixer11;          // xy=797,837
AudioMixer4 mixer13;          // xy=799,913
AudioMixer4 mixer14;          // xy=801,977
AudioMixer4 mixer16;          // xy=803,1046
AudioMixer4 mixer17;          // xy=805,1110
AudioMixer4 mixer3;           // xy=949,395
AudioMixer4 mixer6;           // xy=951,536
AudioMixer4 mixer9;           // xy=951,673
AudioMixer4 mixer12;          // xy=955,803
AudioMixer4 mixer15;          // xy=956,948
AudioMixer4 mixer18;          // xy=957,1075
AudioOutputI2SQuad i2s_quad2; // xy=1324,614
AudioOutputUSB usb2;          // xy=1331,997
AudioAnalyzeRMS rms7;         // xy=1335,1094
AudioAnalyzeRMS rms8;         // xy=1336,1126
AudioAnalyzeRMS rms11;        // xy=1336,1221
AudioAnalyzeRMS rms12;        // xy=1336,1254
AudioAnalyzeRMS rms9;         // xy=1337,1158
AudioAnalyzeRMS rms10;        // xy=1338,1190
AudioAnalyzePeak peak7;       // xy=1342,270
AudioAnalyzePeak peak9;       // xy=1342,335
AudioAnalyzePeak peak10;      // xy=1342,366
AudioAnalyzePeak peak8;       // xy=1343,303
AudioAnalyzePeak peak11;      // xy=1343,397
AudioAnalyzePeak peak12;      // xy=1343,428
AudioConnection patchCord9(i2s_quad1, 2, mixer1, 1);
AudioConnection patchCord10(i2s_quad1, 2, mixer4, 1);
AudioConnection patchCord11(i2s_quad1, 2, mixer7, 1);
AudioConnection patchCord12(i2s_quad1, 2, mixer10, 1);
AudioConnection patchCord13(i2s_quad1, 2, mixer13, 1);
AudioConnection patchCord14(i2s_quad1, 2, mixer16, 1);
AudioConnection patchCord15(i2s_quad1, 2, peak2, 0);
AudioConnection patchCord16(i2s_quad1, 2, rms2, 0);
AudioConnection patchCord1(i2s_quad1, 0, peak1, 0);
AudioConnection patchCord2(i2s_quad1, 0, rms1, 0);
AudioConnection patchCord3(i2s_quad1, 0, mixer1, 0);
AudioConnection patchCord4(i2s_quad1, 0, mixer4, 0);
AudioConnection patchCord5(i2s_quad1, 0, mixer7, 0);
AudioConnection patchCord6(i2s_quad1, 0, mixer10, 0);
AudioConnection patchCord7(i2s_quad1, 0, mixer13, 0);
AudioConnection patchCord8(i2s_quad1, 0, mixer16, 0);
AudioConnection patchCord25(i2s_quad1, 1, mixer1, 3);
AudioConnection patchCord26(i2s_quad1, 1, mixer4, 3);
AudioConnection patchCord27(i2s_quad1, 1, mixer7, 3);
AudioConnection patchCord28(i2s_quad1, 1, mixer10, 3);
AudioConnection patchCord29(i2s_quad1, 1, mixer13, 3);
AudioConnection patchCord30(i2s_quad1, 1, mixer16, 3);
AudioConnection patchCord31(i2s_quad1, 1, peak4, 0);
AudioConnection patchCord32(i2s_quad1, 1, rms4, 0);
AudioConnection patchCord17(i2s_quad1, 3, mixer1, 2);
AudioConnection patchCord18(i2s_quad1, 3, mixer4, 2);
AudioConnection patchCord19(i2s_quad1, 3, mixer7, 2);
AudioConnection patchCord20(i2s_quad1, 3, mixer10, 2);
AudioConnection patchCord21(i2s_quad1, 3, mixer13, 2);
AudioConnection patchCord22(i2s_quad1, 3, mixer16, 2);
AudioConnection patchCord23(i2s_quad1, 3, peak3, 0);
AudioConnection patchCord24(i2s_quad1, 3, rms3, 0);
AudioConnection patchCord33(pink1, 0, mixer2, 3);
AudioConnection patchCord34(pink1, 0, mixer5, 3);
AudioConnection patchCord35(pink1, 0, mixer8, 3);
AudioConnection patchCord36(pink1, 0, mixer11, 3);
AudioConnection patchCord37(pink1, 0, mixer14, 3);
AudioConnection patchCord38(pink1, 0, mixer17, 3);
AudioConnection patchCord39(sine1, 0, mixer2, 2);
AudioConnection patchCord40(sine1, 0, mixer5, 2);
AudioConnection patchCord41(sine1, 0, mixer8, 2);
AudioConnection patchCord42(sine1, 0, mixer11, 2);
AudioConnection patchCord43(sine1, 0, mixer14, 2);
AudioConnection patchCord44(sine1, 0, mixer17, 2);
AudioConnection patchCord45(usb1, 0, mixer2, 0);
AudioConnection patchCord46(usb1, 0, mixer5, 0);
AudioConnection patchCord47(usb1, 0, mixer8, 0);
AudioConnection patchCord48(usb1, 0, mixer11, 0);
AudioConnection patchCord49(usb1, 0, mixer14, 0);
AudioConnection patchCord50(usb1, 0, mixer17, 0);
AudioConnection patchCord51(usb1, 0, peak5, 0);
AudioConnection patchCord52(usb1, 0, rms5, 0);
AudioConnection patchCord53(usb1, 1, mixer2, 1);
AudioConnection patchCord54(usb1, 1, mixer5, 1);
AudioConnection patchCord55(usb1, 1, mixer8, 1);
AudioConnection patchCord56(usb1, 1, mixer11, 1);
AudioConnection patchCord57(usb1, 1, mixer14, 1);
AudioConnection patchCord58(usb1, 1, mixer17, 1);
AudioConnection patchCord59(usb1, 1, peak6, 0);
AudioConnection patchCord60(usb1, 1, rms6, 0);
AudioConnection patchCord61(mixer1, 0, mixer3, 0);
AudioConnection patchCord62(mixer4, 0, mixer6, 0);
AudioConnection patchCord63(mixer2, 0, mixer3, 1);
AudioConnection patchCord64(mixer5, 0, mixer6, 1);
AudioConnection patchCord65(mixer7, 0, mixer9, 0);
AudioConnection patchCord66(mixer8, 0, mixer9, 1);
AudioConnection patchCord67(mixer10, 0, mixer12, 0);
AudioConnection patchCord68(mixer11, 0, mixer12, 1);
AudioConnection patchCord69(mixer13, 0, mixer15, 0);
AudioConnection patchCord70(mixer14, 0, mixer15, 1);
AudioConnection patchCord71(mixer16, 0, mixer18, 0);
AudioConnection patchCord72(mixer17, 0, mixer18, 1);
AudioConnection patchCord73(mixer3, 0, i2s_quad2, 0);
AudioConnection patchCord74(mixer3, rms7);
AudioConnection patchCord75(mixer3, peak7);
AudioConnection patchCord76(mixer6, 0, i2s_quad2, 1);
AudioConnection patchCord77(mixer6, rms8);
AudioConnection patchCord78(mixer6, peak8);
AudioConnection patchCord79(mixer9, 0, i2s_quad2, 2);
AudioConnection patchCord80(mixer9, rms9);
AudioConnection patchCord81(mixer9, peak9);
AudioConnection patchCord82(mixer12, 0, i2s_quad2, 3);
AudioConnection patchCord83(mixer12, rms10);
AudioConnection patchCord84(mixer12, peak10);
AudioConnection patchCord85(mixer15, 0, usb2, 0);
AudioConnection patchCord86(mixer15, rms11);
AudioConnection patchCord87(mixer15, peak11);
AudioConnection patchCord88(mixer18, 0, usb2, 1);
AudioConnection patchCord89(mixer18, rms12);
AudioConnection patchCord90(mixer18, peak12);
// GUItool: end automatically generated code

AudioControlSGTL5000 sgtl5000_1;
AudioControlSGTL5000 sgtl5000_2;

AudioMixer4 *matrix[6][2] = {
    {&mixer1, &mixer2},   {&mixer4, &mixer5},   {&mixer7, &mixer8},
    {&mixer10, &mixer11}, {&mixer13, &mixer14}, {&mixer16, &mixer17},
};

AudioAnalyzeRMS *ent_rms[12] = {
    &rms1, &rms2, &rms3, &rms4,  &rms5,  &rms6,
    &rms7, &rms8, &rms9, &rms10, &rms11, &rms12,
};

AudioAnalyzePeak *ent_peak[12] = {
    &peak1, &peak2, &peak3, &peak4,  &peak5,  &peak6,
    &peak7, &peak8, &peak9, &peak10, &peak11, &peak12,
};

void audio_setup() {
    AudioMemory(64);

    uint8_t adcGain = 0;

    sgtl5000_1.enable();
    sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
    sgtl5000_1.volume(0.5);
    sgtl5000_1.lineOutLevel(13);
    sgtl5000_1.lineInLevel(adcGain, adcGain);
    sgtl5000_1.adcHighPassFilterDisable();
    sgtl5000_1.muteHeadphone();

    sgtl5000_2.setWire(1);

    sgtl5000_2.enable();
    sgtl5000_2.inputSelect(AUDIO_INPUT_LINEIN);
    sgtl5000_2.volume(0.5);
    sgtl5000_2.lineOutLevel(13);
    sgtl5000_2.lineInLevel(adcGain, adcGain);
    sgtl5000_2.adcHighPassFilterDisable();
}

void update_levels(float levels_smooth[CHANNELS + BUSES],
                   float levels_rms[CHANNELS + BUSES],
                   float levels_peak[CHANNELS + BUSES]) {
    float temp;
    if (rms1.available()) {
        for (int i = 0; i < CHANNELS + BUSES; i++) {
            temp = ent_rms[i]->read();
            levels_smooth[i] = ((levels_smooth[i] * 9) + temp) / 10;
            temp = levels_smooth[i];
            // VU meter drains slowly after a peak
            if (temp < levels_rms[i]) {
                levels_rms[i] *= 0.97;
            } else {
                levels_rms[i] = temp;
            }
            levels_peak[i] = ent_peak[i]->read();
        }
    }
}

void raw_set_crosspoint(int channel, int bus, float gain) {
    matrix[bus][channel / 4]->gain(channel % 4, gain);
}

float raw_get_crosspoint(int channel, int bus) {
    return matrix[bus][channel / 4]->getGain(channel % 4);
}

void raw_set_mix(int bus, float in1, float in2, float in3, float in4, float in5,
                 float in6) {
    matrix[bus][0]->gain(0, in1);
    matrix[bus][0]->gain(1, in2);
    matrix[bus][0]->gain(2, in3);
    matrix[bus][0]->gain(3, in4);
    matrix[bus][1]->gain(0, in5);
    matrix[bus][1]->gain(1, in6);
    matrix[bus][1]->gain(2, 0.0f);
    matrix[bus][1]->gain(3, 0.0f);
}

void raw_set_mix(int bus, float bus_gains[CHANNELS]) {
    raw_set_mix(bus, bus_gains[0], bus_gains[1], bus_gains[2], bus_gains[3],
                bus_gains[4], bus_gains[5]);
}
