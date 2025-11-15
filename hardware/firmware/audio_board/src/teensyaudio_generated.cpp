// clang-format off

// GUItool: begin automatically generated code
AudioInputTDM            tdm1;           //xy=131,374
AudioSynthNoisePink      pink1;          //xy=146,697
AudioSynthWaveformSine   sine1;          //xy=148,657
AudioInputUSB            usb1;           //xy=149,616
AudioAnalyzePeak         peak5;          //xy=411,306
AudioAnalyzePeak         peak1;          //xy=412,172
AudioAnalyzePeak         peak2;          //xy=413,205
AudioAnalyzePeak         peak3;          //xy=413,239
AudioAnalyzePeak         peak6;          //xy=413,340
AudioAnalyzePeak         peak4;          //xy=415,272
AudioAnalyzeRMS          rms5;           //xy=426,1119
AudioAnalyzeRMS          rms6;           //xy=428,1152
AudioAnalyzeRMS          rms3;           //xy=429,1053
AudioAnalyzeRMS          rms1;           //xy=430,988
AudioAnalyzeRMS          rms2;           //xy=430,1021
AudioAnalyzeRMS          rms4;           //xy=430,1086
AudioMixer4              mixer1;         //xy=595,237
AudioMixer4              mixer4;         //xy=596,375
AudioMixer4              mixer2;         //xy=597,301
AudioMixer4              mixer5;         //xy=598,439
AudioMixer4              mixer7;         //xy=599,513
AudioMixer4              mixer8;         //xy=601,577
AudioMixer4              mixer10;        //xy=603,646
AudioMixer4              mixer11;        //xy=605,710
AudioMixer4              mixer13;        //xy=607,786
AudioMixer4              mixer14;        //xy=609,850
AudioMixer4              mixer16;        //xy=611,919
AudioMixer4              mixer17;        //xy=613,983
AudioMixer4              mixer3;         //xy=757,268
AudioMixer4              mixer6;         //xy=759,409
AudioMixer4              mixer9;         //xy=759,546
AudioMixer4              mixer12;        //xy=763,676
AudioMixer4              mixer15;        //xy=764,821
AudioMixer4              mixer18;        //xy=765,948
AudioOutputUSB           usb2;           //xy=1139,870
AudioAnalyzeRMS          rms7;           //xy=1143,967
AudioAnalyzeRMS          rms8;           //xy=1144,999
AudioAnalyzeRMS          rms11;          //xy=1144,1094
AudioAnalyzeRMS          rms12;          //xy=1144,1127
AudioAnalyzeRMS          rms9;           //xy=1145,1031
AudioAnalyzeRMS          rms10;          //xy=1146,1063
AudioAnalyzePeak         peak7;          //xy=1150,143
AudioAnalyzePeak         peak9;          //xy=1150,208
AudioAnalyzePeak         peak10;         //xy=1150,239
AudioAnalyzePeak         peak8;          //xy=1151,176
AudioAnalyzePeak         peak11;         //xy=1151,270
AudioAnalyzePeak         peak12;         //xy=1151,301
AudioOutputTDM           tdm2;           //xy=1155,613
AudioConnection          patchCord1(tdm1, 0, peak1, 0);
AudioConnection          patchCord2(tdm1, 0, rms1, 0);
AudioConnection          patchCord3(tdm1, 0, mixer1, 0);
AudioConnection          patchCord4(tdm1, 0, mixer4, 0);
AudioConnection          patchCord5(tdm1, 0, mixer7, 0);
AudioConnection          patchCord6(tdm1, 0, mixer10, 0);
AudioConnection          patchCord7(tdm1, 0, mixer13, 0);
AudioConnection          patchCord8(tdm1, 0, mixer16, 0);
AudioConnection          patchCord9(tdm1, 1, peak2, 0);
AudioConnection          patchCord10(tdm1, 1, rms2, 0);
AudioConnection          patchCord11(tdm1, 1, mixer1, 1);
AudioConnection          patchCord12(tdm1, 1, mixer4, 1);
AudioConnection          patchCord13(tdm1, 1, mixer7, 1);
AudioConnection          patchCord14(tdm1, 1, mixer10, 1);
AudioConnection          patchCord15(tdm1, 1, mixer13, 1);
AudioConnection          patchCord16(tdm1, 1, mixer16, 1);
AudioConnection          patchCord17(tdm1, 2, peak3, 0);
AudioConnection          patchCord18(tdm1, 2, rms3, 0);
AudioConnection          patchCord19(tdm1, 2, mixer1, 2);
AudioConnection          patchCord20(tdm1, 2, mixer4, 2);
AudioConnection          patchCord21(tdm1, 2, mixer7, 2);
AudioConnection          patchCord22(tdm1, 2, mixer10, 2);
AudioConnection          patchCord23(tdm1, 2, mixer13, 2);
AudioConnection          patchCord24(tdm1, 2, mixer16, 2);
AudioConnection          patchCord25(tdm1, 3, peak4, 0);
AudioConnection          patchCord26(tdm1, 3, rms4, 0);
AudioConnection          patchCord27(tdm1, 3, mixer1, 3);
AudioConnection          patchCord28(tdm1, 3, mixer4, 3);
AudioConnection          patchCord29(tdm1, 3, mixer7, 3);
AudioConnection          patchCord30(tdm1, 3, mixer10, 3);
AudioConnection          patchCord31(tdm1, 3, mixer13, 3);
AudioConnection          patchCord32(tdm1, 3, mixer16, 3);
AudioConnection          patchCord33(pink1, 0, mixer2, 3);
AudioConnection          patchCord34(pink1, 0, mixer5, 3);
AudioConnection          patchCord35(pink1, 0, mixer8, 3);
AudioConnection          patchCord36(pink1, 0, mixer11, 3);
AudioConnection          patchCord37(pink1, 0, mixer14, 3);
AudioConnection          patchCord38(pink1, 0, mixer17, 3);
AudioConnection          patchCord39(sine1, 0, mixer2, 2);
AudioConnection          patchCord40(sine1, 0, mixer5, 2);
AudioConnection          patchCord41(sine1, 0, mixer8, 2);
AudioConnection          patchCord42(sine1, 0, mixer11, 2);
AudioConnection          patchCord43(sine1, 0, mixer14, 2);
AudioConnection          patchCord44(sine1, 0, mixer17, 2);
AudioConnection          patchCord45(usb1, 0, mixer2, 0);
AudioConnection          patchCord46(usb1, 0, mixer5, 0);
AudioConnection          patchCord47(usb1, 0, mixer8, 0);
AudioConnection          patchCord48(usb1, 0, mixer11, 0);
AudioConnection          patchCord49(usb1, 0, mixer14, 0);
AudioConnection          patchCord50(usb1, 0, mixer17, 0);
AudioConnection          patchCord51(usb1, 0, peak5, 0);
AudioConnection          patchCord52(usb1, 0, rms5, 0);
AudioConnection          patchCord53(usb1, 1, mixer2, 1);
AudioConnection          patchCord54(usb1, 1, mixer5, 1);
AudioConnection          patchCord55(usb1, 1, mixer8, 1);
AudioConnection          patchCord56(usb1, 1, mixer11, 1);
AudioConnection          patchCord57(usb1, 1, mixer14, 1);
AudioConnection          patchCord58(usb1, 1, mixer17, 1);
AudioConnection          patchCord59(usb1, 1, peak6, 0);
AudioConnection          patchCord60(usb1, 1, rms6, 0);
AudioConnection          patchCord61(mixer1, 0, mixer3, 0);
AudioConnection          patchCord62(mixer4, 0, mixer6, 0);
AudioConnection          patchCord63(mixer2, 0, mixer3, 1);
AudioConnection          patchCord64(mixer5, 0, mixer6, 1);
AudioConnection          patchCord65(mixer7, 0, mixer9, 0);
AudioConnection          patchCord66(mixer8, 0, mixer9, 1);
AudioConnection          patchCord67(mixer10, 0, mixer12, 0);
AudioConnection          patchCord68(mixer11, 0, mixer12, 1);
AudioConnection          patchCord69(mixer13, 0, mixer15, 0);
AudioConnection          patchCord70(mixer14, 0, mixer15, 1);
AudioConnection          patchCord71(mixer16, 0, mixer18, 0);
AudioConnection          patchCord72(mixer17, 0, mixer18, 1);
AudioConnection          patchCord73(mixer3, rms7);
AudioConnection          patchCord74(mixer3, peak7);
AudioConnection          patchCord75(mixer3, 0, tdm2, 12);
AudioConnection          patchCord76(mixer6, rms8);
AudioConnection          patchCord77(mixer6, peak8);
AudioConnection          patchCord78(mixer6, 0, tdm2, 14);
AudioConnection          patchCord79(mixer9, rms9);
AudioConnection          patchCord80(mixer9, peak9);
AudioConnection          patchCord81(mixer9, 0, tdm2, 8);
AudioConnection          patchCord82(mixer12, rms10);
AudioConnection          patchCord83(mixer12, peak10);
AudioConnection          patchCord84(mixer15, 0, usb2, 0);
AudioConnection          patchCord85(mixer15, rms11);
AudioConnection          patchCord86(mixer15, peak11);
AudioConnection          patchCord87(mixer15, 0, tdm2, 10);
AudioConnection          patchCord88(mixer18, 0, usb2, 1);
AudioConnection          patchCord89(mixer18, rms12);
AudioConnection          patchCord90(mixer18, peak12);
// GUItool: end automatically generated code

// clang-format on
