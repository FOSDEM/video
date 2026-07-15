// clang-format off

// GUItool: begin automatically generated code
AudioInputTDM2  tdm1;    //xy=84,425
AudioInputUSB  usb1;    //xy=84,1219
AsyncAudioInputSPDIF3  spdif1;    //xy=84,1445
AudioSynthNoisePink  pink1;    //xy=84,1984
AudioSynthWaveformSine  sine1;    //xy=84,2041
AudioAnalyzePeak  peak1;    //xy=346,0
AudioAnalyzeRMS  rms1;    //xy=346,56
AudioAnalyzePeak  peak2;    //xy=346,113
AudioAnalyzeRMS  rms2;    //xy=346,170
AudioAnalyzePeak  peak3;    //xy=346,226
AudioAnalyzeRMS  rms3;    //xy=346,283
AudioAnalyzePeak  peak4;    //xy=346,340
AudioAnalyzeRMS  rms4;    //xy=346,396
AudioMixer4  mixer1;    //xy=346,453
AudioMixer4  mixer5;    //xy=346,510
AudioMixer4  mixer9;    //xy=346,567
AudioMixer4  mixer13;    //xy=346,623
AudioMixer4  mixer17;    //xy=346,680
AudioMixer4  mixer21;    //xy=346,737
AudioMixer4  mixer25;    //xy=346,850
AudioMixer4  mixer29;    //xy=346,793
AudioAnalyzePeak  peak5;    //xy=346,907
AudioAnalyzeRMS  rms5;    //xy=346,963
AudioAnalyzePeak  peak6;    //xy=346,1020
AudioAnalyzeRMS  rms6;    //xy=346,1077
AudioAnalyzePeak  peak7;    //xy=346,1587
AudioAnalyzeRMS  rms7;    //xy=346,1644
AudioAnalyzePeak  peak8;    //xy=346,1701
AudioAnalyzeRMS  rms8;    //xy=346,1757
AudioMixer4  mixer2;    //xy=346,1134
AudioMixer4  mixer6;    //xy=346,1190
AudioMixer4  mixer10;    //xy=346,1247
AudioMixer4  mixer14;    //xy=346,1304
AudioMixer4  mixer18;    //xy=346,1360
AudioMixer4  mixer22;    //xy=346,1417
AudioMixer4  mixer26;    //xy=346,1530
AudioMixer4  mixer30;    //xy=346,1474
AudioMixer4  mixer3;    //xy=346,1814
AudioMixer4  mixer7;    //xy=346,1871
AudioMixer4  mixer11;    //xy=346,1927
AudioMixer4  mixer15;    //xy=346,1984
AudioMixer4  mixer19;    //xy=346,2041
AudioMixer4  mixer23;    //xy=346,2097
AudioMixer4  mixer27;    //xy=346,2211
AudioMixer4  mixer31;    //xy=346,2154
AudioMixer4  mixer4;    //xy=629,950
AudioMixer4  mixer8;    //xy=629,1063
AudioMixer4  mixer12;    //xy=629,1148
AudioMixer4  mixer16;    //xy=629,1275
AudioMixer4  mixer20;    //xy=629,1389
AudioMixer4  mixer24;    //xy=629,1502
AudioMixer4  mixer28;    //xy=629,1786
AudioMixer4  mixer32;    //xy=629,1672
AudioAnalyzePeak  peak9;    //xy=931,850
AudioAnalyzeRMS  rms9;    //xy=931,907
AudioAnalyzePeak  peak10;    //xy=931,963
AudioAnalyzeRMS  rms10;    //xy=931,1020
AudioAnalyzePeak  peak11;    //xy=931,1134
AudioAnalyzeRMS  rms11;    //xy=931,1190
AudioAnalyzePeak  peak12;    //xy=931,1247
AudioAnalyzeRMS  rms12;    //xy=931,1304
AudioOutputI2SQuad  i2s_quad1;    //xy=931,1077
AudioAnalyzePeak  peak13;    //xy=931,1360
AudioAnalyzeRMS  rms13;    //xy=931,1417
AudioAnalyzePeak  peak14;    //xy=931,1530
AudioAnalyzeRMS  rms14;    //xy=931,1587
AudioOutputUSB  usb2;    //xy=931,1474
AudioAnalyzePeak  peak15;    //xy=931,1814
AudioAnalyzeRMS  rms15;    //xy=931,1871
AudioAnalyzePeak  peak16;    //xy=931,1644
AudioAnalyzeRMS  rms16;    //xy=931,1701
AudioOutputSPDIF3  spdif2;    //xy=931,1757
AudioConnection patchCord0(tdm1, 0, peak1, 0);
AudioConnection patchCord1(tdm1, 0, rms1, 0);
AudioConnection patchCord2(tdm1, 1, peak2, 0);
AudioConnection patchCord3(tdm1, 1, rms2, 0);
AudioConnection patchCord4(tdm1, 2, peak3, 0);
AudioConnection patchCord5(tdm1, 2, rms3, 0);
AudioConnection patchCord6(tdm1, 3, peak4, 0);
AudioConnection patchCord7(tdm1, 3, rms4, 0);
AudioConnection patchCord8(usb1, 0, peak5, 0);
AudioConnection patchCord9(usb1, 0, rms5, 0);
AudioConnection patchCord10(usb1, 1, peak6, 0);
AudioConnection patchCord11(usb1, 1, rms6, 0);
AudioConnection patchCord12(spdif1, 0, peak7, 0);
AudioConnection patchCord13(spdif1, 0, rms7, 0);
AudioConnection patchCord14(spdif1, 1, peak8, 0);
AudioConnection patchCord15(spdif1, 1, rms8, 0);
AudioConnection patchCord16(tdm1, 0, mixer1, 0);
AudioConnection patchCord17(tdm1, 1, mixer1, 1);
AudioConnection patchCord18(tdm1, 2, mixer1, 2);
AudioConnection patchCord19(tdm1, 3, mixer1, 3);
AudioConnection patchCord20(usb1, 0, mixer2, 0);
AudioConnection patchCord21(usb1, 1, mixer2, 1);
AudioConnection patchCord22(spdif1, 0, mixer2, 2);
AudioConnection patchCord23(spdif1, 1, mixer2, 3);
AudioConnection patchCord24(pink1, 0, mixer3, 0);
AudioConnection patchCord25(sine1, 0, mixer3, 1);
AudioConnection patchCord26(mixer1, 0, mixer4, 0);
AudioConnection patchCord27(mixer2, 0, mixer4, 1);
AudioConnection patchCord28(mixer3, 0, mixer4, 2);
AudioConnection patchCord29(mixer4, 0, peak9, 0);
AudioConnection patchCord30(mixer4, 0, rms9, 0);
AudioConnection patchCord31(mixer4, 0, i2s_quad1, 0);
AudioConnection patchCord32(tdm1, 0, mixer5, 0);
AudioConnection patchCord33(tdm1, 1, mixer5, 1);
AudioConnection patchCord34(tdm1, 2, mixer5, 2);
AudioConnection patchCord35(tdm1, 3, mixer5, 3);
AudioConnection patchCord36(usb1, 0, mixer6, 0);
AudioConnection patchCord37(usb1, 1, mixer6, 1);
AudioConnection patchCord38(spdif1, 0, mixer6, 2);
AudioConnection patchCord39(spdif1, 1, mixer6, 3);
AudioConnection patchCord40(pink1, 0, mixer7, 0);
AudioConnection patchCord41(sine1, 0, mixer7, 1);
AudioConnection patchCord42(mixer5, 0, mixer8, 0);
AudioConnection patchCord43(mixer6, 0, mixer8, 1);
AudioConnection patchCord44(mixer7, 0, mixer8, 2);
AudioConnection patchCord45(mixer8, 0, peak10, 0);
AudioConnection patchCord46(mixer8, 0, rms10, 0);
AudioConnection patchCord47(mixer8, 0, i2s_quad1, 1);
AudioConnection patchCord48(tdm1, 0, mixer9, 0);
AudioConnection patchCord49(tdm1, 1, mixer9, 1);
AudioConnection patchCord50(tdm1, 2, mixer9, 2);
AudioConnection patchCord51(tdm1, 3, mixer9, 3);
AudioConnection patchCord52(usb1, 0, mixer10, 0);
AudioConnection patchCord53(usb1, 1, mixer10, 1);
AudioConnection patchCord54(spdif1, 0, mixer10, 2);
AudioConnection patchCord55(spdif1, 1, mixer10, 3);
AudioConnection patchCord56(pink1, 0, mixer11, 0);
AudioConnection patchCord57(sine1, 0, mixer11, 1);
AudioConnection patchCord58(mixer9, 0, mixer12, 0);
AudioConnection patchCord59(mixer10, 0, mixer12, 1);
AudioConnection patchCord60(mixer11, 0, mixer12, 2);
AudioConnection patchCord61(mixer12, 0, peak11, 0);
AudioConnection patchCord62(mixer12, 0, rms11, 0);
AudioConnection patchCord63(mixer12, 0, i2s_quad1, 2);
AudioConnection patchCord64(tdm1, 0, mixer13, 0);
AudioConnection patchCord65(tdm1, 1, mixer13, 1);
AudioConnection patchCord66(tdm1, 2, mixer13, 2);
AudioConnection patchCord67(tdm1, 3, mixer13, 3);
AudioConnection patchCord68(usb1, 0, mixer14, 0);
AudioConnection patchCord69(usb1, 1, mixer14, 1);
AudioConnection patchCord70(spdif1, 0, mixer14, 2);
AudioConnection patchCord71(spdif1, 1, mixer14, 3);
AudioConnection patchCord72(pink1, 0, mixer15, 0);
AudioConnection patchCord73(sine1, 0, mixer15, 1);
AudioConnection patchCord74(mixer13, 0, mixer16, 0);
AudioConnection patchCord75(mixer14, 0, mixer16, 1);
AudioConnection patchCord76(mixer15, 0, mixer16, 2);
AudioConnection patchCord77(mixer16, 0, peak12, 0);
AudioConnection patchCord78(mixer16, 0, rms12, 0);
AudioConnection patchCord79(mixer16, 0, i2s_quad1, 3);
AudioConnection patchCord80(tdm1, 0, mixer17, 0);
AudioConnection patchCord81(tdm1, 1, mixer17, 1);
AudioConnection patchCord82(tdm1, 2, mixer17, 2);
AudioConnection patchCord83(tdm1, 3, mixer17, 3);
AudioConnection patchCord84(usb1, 0, mixer18, 0);
AudioConnection patchCord85(usb1, 1, mixer18, 1);
AudioConnection patchCord86(spdif1, 0, mixer18, 2);
AudioConnection patchCord87(spdif1, 1, mixer18, 3);
AudioConnection patchCord88(pink1, 0, mixer19, 0);
AudioConnection patchCord89(sine1, 0, mixer19, 1);
AudioConnection patchCord90(mixer17, 0, mixer20, 0);
AudioConnection patchCord91(mixer18, 0, mixer20, 1);
AudioConnection patchCord92(mixer19, 0, mixer20, 2);
AudioConnection patchCord93(mixer20, 0, peak13, 0);
AudioConnection patchCord94(mixer20, 0, rms13, 0);
AudioConnection patchCord95(mixer20, 0, usb2, 0);
AudioConnection patchCord96(tdm1, 0, mixer21, 0);
AudioConnection patchCord97(tdm1, 1, mixer21, 1);
AudioConnection patchCord98(tdm1, 2, mixer21, 2);
AudioConnection patchCord99(tdm1, 3, mixer21, 3);
AudioConnection patchCord100(usb1, 0, mixer22, 0);
AudioConnection patchCord101(usb1, 1, mixer22, 1);
AudioConnection patchCord102(spdif1, 0, mixer22, 2);
AudioConnection patchCord103(spdif1, 1, mixer22, 3);
AudioConnection patchCord104(pink1, 0, mixer23, 0);
AudioConnection patchCord105(sine1, 0, mixer23, 1);
AudioConnection patchCord106(mixer21, 0, mixer24, 0);
AudioConnection patchCord107(mixer22, 0, mixer24, 1);
AudioConnection patchCord108(mixer23, 0, mixer24, 2);
AudioConnection patchCord109(mixer24, 0, peak14, 0);
AudioConnection patchCord110(mixer24, 0, rms14, 0);
AudioConnection patchCord111(mixer24, 0, usb2, 1);
AudioConnection patchCord112(tdm1, 0, mixer25, 0);
AudioConnection patchCord113(tdm1, 1, mixer25, 1);
AudioConnection patchCord114(tdm1, 2, mixer25, 2);
AudioConnection patchCord115(tdm1, 3, mixer25, 3);
AudioConnection patchCord116(usb1, 0, mixer26, 0);
AudioConnection patchCord117(usb1, 1, mixer26, 1);
AudioConnection patchCord118(spdif1, 0, mixer26, 2);
AudioConnection patchCord119(spdif1, 1, mixer26, 3);
AudioConnection patchCord120(pink1, 0, mixer27, 0);
AudioConnection patchCord121(sine1, 0, mixer27, 1);
AudioConnection patchCord122(mixer25, 0, mixer28, 0);
AudioConnection patchCord123(mixer26, 0, mixer28, 1);
AudioConnection patchCord124(mixer27, 0, mixer28, 2);
AudioConnection patchCord125(mixer28, 0, peak15, 0);
AudioConnection patchCord126(mixer28, 0, rms15, 0);
AudioConnection patchCord127(mixer28, 0, spdif2, 0);
AudioConnection patchCord128(tdm1, 0, mixer29, 0);
AudioConnection patchCord129(tdm1, 1, mixer29, 1);
AudioConnection patchCord130(tdm1, 2, mixer29, 2);
AudioConnection patchCord131(tdm1, 3, mixer29, 3);
AudioConnection patchCord132(usb1, 0, mixer30, 0);
AudioConnection patchCord133(usb1, 1, mixer30, 1);
AudioConnection patchCord134(spdif1, 0, mixer30, 2);
AudioConnection patchCord135(spdif1, 1, mixer30, 3);
AudioConnection patchCord136(pink1, 0, mixer31, 0);
AudioConnection patchCord137(sine1, 0, mixer31, 1);
AudioConnection patchCord138(mixer29, 0, mixer32, 0);
AudioConnection patchCord139(mixer30, 0, mixer32, 1);
AudioConnection patchCord140(mixer31, 0, mixer32, 2);
AudioConnection patchCord141(mixer32, 0, peak16, 0);
AudioConnection patchCord142(mixer32, 0, rms16, 0);
AudioConnection patchCord143(mixer32, 0, spdif2, 1);
// GUItool: end automatically generated code
