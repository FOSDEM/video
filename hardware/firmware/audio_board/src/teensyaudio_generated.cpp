// clang-format off

#include "channel_strip.h"
// GUItool: begin automatically generated code
AudioInputTDM2  tdm1;    //xy=84,311
AudioInputUSB  usb1;    //xy=84,1615
AsyncAudioInputSPDIF3  spdif1;    //xy=84,1275
AudioSynthNoisePink  pink1;    //xy=344,453
AudioSynthWaveformSine  sine1;    //xy=344,510
AudioAnalyzePeak  peak1;    //xy=344,113
AudioAnalyzeRMS  rms1;    //xy=344,170
AudioFilterBiquad  biquad1;    //xy=344,765
AudioAnalyzePeak  peak2;    //xy=344,226
AudioAnalyzeRMS  rms2;    //xy=344,283
AudioFilterBiquad  biquad2;    //xy=344,822
AudioAnalyzePeak  peak3;    //xy=344,340
AudioAnalyzeRMS  rms3;    //xy=344,396
AudioFilterBiquad  biquad3;    //xy=344,878
AudioAnalyzePeak  peak4;    //xy=344,0
AudioAnalyzeRMS  rms4;    //xy=344,56
AudioFilterBiquad  biquad4;    //xy=344,935
AudioAnalyzePeak  peak5;    //xy=344,1587
AudioAnalyzeRMS  rms5;    //xy=344,1644
AudioFilterBiquad  biquad5;    //xy=344,1474
AudioAnalyzePeak  peak6;    //xy=344,1701
AudioAnalyzeRMS  rms6;    //xy=344,1757
AudioFilterBiquad  biquad6;    //xy=344,1530
AudioAnalyzePeak  peak7;    //xy=344,1360
AudioAnalyzeRMS  rms7;    //xy=344,1417
AudioFilterBiquad  biquad7;    //xy=344,1134
AudioAnalyzePeak  peak8;    //xy=344,1247
AudioAnalyzeRMS  rms8;    //xy=344,1304
AudioFilterBiquad  biquad8;    //xy=344,1190
AudioMixer4  mixer3;    //xy=625,226
AudioMixer4  mixer7;    //xy=625,283
AudioMixer4  mixer11;    //xy=625,340
AudioMixer4  mixer15;    //xy=625,396
AudioMixer4  mixer19;    //xy=625,453
AudioMixer4  mixer23;    //xy=625,510
AudioMixer4  mixer27;    //xy=625,567
AudioMixer4  mixer31;    //xy=625,623
AudioMixer4  mixer1;    //xy=625,680
AudioMixer4  mixer5;    //xy=625,737
AudioMixer4  mixer9;    //xy=625,793
AudioMixer4  mixer13;    //xy=625,850
AudioMixer4  mixer17;    //xy=625,907
AudioMixer4  mixer21;    //xy=625,963
AudioMixer4  mixer25;    //xy=625,1020
AudioMixer4  mixer29;    //xy=625,1077
AudioMixer4  mixer2;    //xy=625,1134
AudioMixer4  mixer6;    //xy=625,1190
AudioMixer4  mixer10;    //xy=625,1247
AudioMixer4  mixer14;    //xy=625,1304
AudioMixer4  mixer18;    //xy=625,1360
AudioMixer4  mixer22;    //xy=625,1417
AudioMixer4  mixer26;    //xy=625,1474
AudioMixer4  mixer30;    //xy=625,1530
AudioMixer4  mixer4;    //xy=908,593
AudioMixer4  mixer8;    //xy=908,678
AudioMixer4  mixer12;    //xy=908,757
AudioMixer4  mixer16;    //xy=908,829
AudioMixer4  mixer20;    //xy=908,928
AudioMixer4  mixer24;    //xy=908,991
AudioMixer4  mixer28;    //xy=908,1089
AudioMixer4  mixer32;    //xy=908,1223
AudioFilterBiquad  biquad9;    //xy=1202,500
AudioFilterBiquad  biquad10;    //xy=1202,599
AudioFilterBiquad  biquad11;    //xy=1202,712
AudioFilterBiquad  biquad12;    //xy=1202,808
AudioFilterBiquad  biquad13;    //xy=1202,962
AudioFilterBiquad  biquad14;    //xy=1202,1047
AudioFilterBiquad  biquad15;    //xy=1202,1223
AudioFilterBiquad  biquad16;    //xy=1202,1336
AudioAnalyzePeak  peak9;    //xy=1514,457
AudioAnalyzeRMS  rms9;    //xy=1514,401
AudioAnalyzePeak  peak10;    //xy=1514,571
AudioAnalyzeRMS  rms10;    //xy=1514,514
AudioAnalyzePeak  peak11;    //xy=1514,741
AudioAnalyzeRMS  rms11;    //xy=1514,684
AudioAnalyzePeak  peak12;    //xy=1514,854
AudioAnalyzeRMS  rms12;    //xy=1514,798
AudioOutputI2SQuad  i2s_quad1;    //xy=1514,627
AudioAnalyzePeak  peak13;    //xy=1514,968
AudioAnalyzeRMS  rms13;    //xy=1514,911
AudioAnalyzePeak  peak14;    //xy=1514,1081
AudioAnalyzeRMS  rms14;    //xy=1514,1138
AudioOutputUSB  usb2;    //xy=1514,1024
AudioAnalyzePeak  peak15;    //xy=1514,1251
AudioAnalyzeRMS  rms15;    //xy=1514,1194
AudioAnalyzePeak  peak16;    //xy=1514,1365
AudioAnalyzeRMS  rms16;    //xy=1514,1421
AudioOutputSPDIF3  spdif2;    //xy=1514,1308
AudioConnection patchCord0(tdm1, 0, peak1, 0);
AudioConnection patchCord1(tdm1, 0, rms1, 0);
AudioConnection patchCord2(tdm1, 0, biquad1, 0);
AudioConnection patchCord3(tdm1, 1, peak2, 0);
AudioConnection patchCord4(tdm1, 1, rms2, 0);
AudioConnection patchCord5(tdm1, 1, biquad2, 0);
AudioConnection patchCord6(tdm1, 2, peak3, 0);
AudioConnection patchCord7(tdm1, 2, rms3, 0);
AudioConnection patchCord8(tdm1, 2, biquad3, 0);
AudioConnection patchCord9(tdm1, 3, peak4, 0);
AudioConnection patchCord10(tdm1, 3, rms4, 0);
AudioConnection patchCord11(tdm1, 3, biquad4, 0);
AudioConnection patchCord12(usb1, 0, peak5, 0);
AudioConnection patchCord13(usb1, 0, rms5, 0);
AudioConnection patchCord14(usb1, 0, biquad5, 0);
AudioConnection patchCord15(usb1, 1, peak6, 0);
AudioConnection patchCord16(usb1, 1, rms6, 0);
AudioConnection patchCord17(usb1, 1, biquad6, 0);
AudioConnection patchCord18(spdif1, 0, peak7, 0);
AudioConnection patchCord19(spdif1, 0, rms7, 0);
AudioConnection patchCord20(spdif1, 0, biquad7, 0);
AudioConnection patchCord21(spdif1, 1, peak8, 0);
AudioConnection patchCord22(spdif1, 1, rms8, 0);
AudioConnection patchCord23(spdif1, 1, biquad8, 0);
AudioConnection patchCord24(biquad9, 0, peak9, 0);
AudioConnection patchCord25(biquad9, 0, rms9, 0);
AudioConnection patchCord26(biquad9, 0, i2s_quad1, 0);
AudioConnection patchCord27(biquad10, 0, peak10, 0);
AudioConnection patchCord28(biquad10, 0, rms10, 0);
AudioConnection patchCord29(biquad10, 0, i2s_quad1, 1);
AudioConnection patchCord30(biquad11, 0, peak11, 0);
AudioConnection patchCord31(biquad11, 0, rms11, 0);
AudioConnection patchCord32(biquad11, 0, i2s_quad1, 2);
AudioConnection patchCord33(biquad12, 0, peak12, 0);
AudioConnection patchCord34(biquad12, 0, rms12, 0);
AudioConnection patchCord35(biquad12, 0, i2s_quad1, 3);
AudioConnection patchCord36(biquad13, 0, peak13, 0);
AudioConnection patchCord37(biquad13, 0, rms13, 0);
AudioConnection patchCord38(biquad13, 0, usb2, 0);
AudioConnection patchCord39(biquad14, 0, peak14, 0);
AudioConnection patchCord40(biquad14, 0, rms14, 0);
AudioConnection patchCord41(biquad14, 0, usb2, 1);
AudioConnection patchCord42(biquad15, 0, peak15, 0);
AudioConnection patchCord43(biquad15, 0, rms15, 0);
AudioConnection patchCord44(biquad15, 0, spdif2, 0);
AudioConnection patchCord45(biquad16, 0, peak16, 0);
AudioConnection patchCord46(biquad16, 0, rms16, 0);
AudioConnection patchCord47(biquad16, 0, spdif2, 1);
AudioConnection patchCord48(biquad1, 0, mixer1, 0);
AudioConnection patchCord49(biquad2, 0, mixer1, 1);
AudioConnection patchCord50(biquad3, 0, mixer1, 2);
AudioConnection patchCord51(biquad4, 0, mixer1, 3);
AudioConnection patchCord52(biquad5, 0, mixer2, 0);
AudioConnection patchCord53(biquad6, 0, mixer2, 1);
AudioConnection patchCord54(biquad7, 0, mixer2, 2);
AudioConnection patchCord55(biquad8, 0, mixer2, 3);
AudioConnection patchCord56(pink1, 0, mixer3, 0);
AudioConnection patchCord57(sine1, 0, mixer3, 1);
AudioConnection patchCord58(mixer1, 0, mixer4, 0);
AudioConnection patchCord59(mixer2, 0, mixer4, 1);
AudioConnection patchCord60(mixer3, 0, mixer4, 2);
AudioConnection patchCord61(mixer4, 0, biquad9, 0);
AudioConnection patchCord62(biquad1, 0, mixer5, 0);
AudioConnection patchCord63(biquad2, 0, mixer5, 1);
AudioConnection patchCord64(biquad3, 0, mixer5, 2);
AudioConnection patchCord65(biquad4, 0, mixer5, 3);
AudioConnection patchCord66(biquad5, 0, mixer6, 0);
AudioConnection patchCord67(biquad6, 0, mixer6, 1);
AudioConnection patchCord68(biquad7, 0, mixer6, 2);
AudioConnection patchCord69(biquad8, 0, mixer6, 3);
AudioConnection patchCord70(pink1, 0, mixer7, 0);
AudioConnection patchCord71(sine1, 0, mixer7, 1);
AudioConnection patchCord72(mixer5, 0, mixer8, 0);
AudioConnection patchCord73(mixer6, 0, mixer8, 1);
AudioConnection patchCord74(mixer7, 0, mixer8, 2);
AudioConnection patchCord75(mixer8, 0, biquad10, 0);
AudioConnection patchCord76(biquad1, 0, mixer9, 0);
AudioConnection patchCord77(biquad2, 0, mixer9, 1);
AudioConnection patchCord78(biquad3, 0, mixer9, 2);
AudioConnection patchCord79(biquad4, 0, mixer9, 3);
AudioConnection patchCord80(biquad5, 0, mixer10, 0);
AudioConnection patchCord81(biquad6, 0, mixer10, 1);
AudioConnection patchCord82(biquad7, 0, mixer10, 2);
AudioConnection patchCord83(biquad8, 0, mixer10, 3);
AudioConnection patchCord84(pink1, 0, mixer11, 0);
AudioConnection patchCord85(sine1, 0, mixer11, 1);
AudioConnection patchCord86(mixer9, 0, mixer12, 0);
AudioConnection patchCord87(mixer10, 0, mixer12, 1);
AudioConnection patchCord88(mixer11, 0, mixer12, 2);
AudioConnection patchCord89(mixer12, 0, biquad11, 0);
AudioConnection patchCord90(biquad1, 0, mixer13, 0);
AudioConnection patchCord91(biquad2, 0, mixer13, 1);
AudioConnection patchCord92(biquad3, 0, mixer13, 2);
AudioConnection patchCord93(biquad4, 0, mixer13, 3);
AudioConnection patchCord94(biquad5, 0, mixer14, 0);
AudioConnection patchCord95(biquad6, 0, mixer14, 1);
AudioConnection patchCord96(biquad7, 0, mixer14, 2);
AudioConnection patchCord97(biquad8, 0, mixer14, 3);
AudioConnection patchCord98(pink1, 0, mixer15, 0);
AudioConnection patchCord99(sine1, 0, mixer15, 1);
AudioConnection patchCord100(mixer13, 0, mixer16, 0);
AudioConnection patchCord101(mixer14, 0, mixer16, 1);
AudioConnection patchCord102(mixer15, 0, mixer16, 2);
AudioConnection patchCord103(mixer16, 0, biquad12, 0);
AudioConnection patchCord104(biquad1, 0, mixer17, 0);
AudioConnection patchCord105(biquad2, 0, mixer17, 1);
AudioConnection patchCord106(biquad3, 0, mixer17, 2);
AudioConnection patchCord107(biquad4, 0, mixer17, 3);
AudioConnection patchCord108(biquad5, 0, mixer18, 0);
AudioConnection patchCord109(biquad6, 0, mixer18, 1);
AudioConnection patchCord110(biquad7, 0, mixer18, 2);
AudioConnection patchCord111(biquad8, 0, mixer18, 3);
AudioConnection patchCord112(pink1, 0, mixer19, 0);
AudioConnection patchCord113(sine1, 0, mixer19, 1);
AudioConnection patchCord114(mixer17, 0, mixer20, 0);
AudioConnection patchCord115(mixer18, 0, mixer20, 1);
AudioConnection patchCord116(mixer19, 0, mixer20, 2);
AudioConnection patchCord117(mixer20, 0, biquad13, 0);
AudioConnection patchCord118(biquad1, 0, mixer21, 0);
AudioConnection patchCord119(biquad2, 0, mixer21, 1);
AudioConnection patchCord120(biquad3, 0, mixer21, 2);
AudioConnection patchCord121(biquad4, 0, mixer21, 3);
AudioConnection patchCord122(biquad5, 0, mixer22, 0);
AudioConnection patchCord123(biquad6, 0, mixer22, 1);
AudioConnection patchCord124(biquad7, 0, mixer22, 2);
AudioConnection patchCord125(biquad8, 0, mixer22, 3);
AudioConnection patchCord126(pink1, 0, mixer23, 0);
AudioConnection patchCord127(sine1, 0, mixer23, 1);
AudioConnection patchCord128(mixer21, 0, mixer24, 0);
AudioConnection patchCord129(mixer22, 0, mixer24, 1);
AudioConnection patchCord130(mixer23, 0, mixer24, 2);
AudioConnection patchCord131(mixer24, 0, biquad14, 0);
AudioConnection patchCord132(biquad1, 0, mixer25, 0);
AudioConnection patchCord133(biquad2, 0, mixer25, 1);
AudioConnection patchCord134(biquad3, 0, mixer25, 2);
AudioConnection patchCord135(biquad4, 0, mixer25, 3);
AudioConnection patchCord136(biquad5, 0, mixer26, 0);
AudioConnection patchCord137(biquad6, 0, mixer26, 1);
AudioConnection patchCord138(biquad7, 0, mixer26, 2);
AudioConnection patchCord139(biquad8, 0, mixer26, 3);
AudioConnection patchCord140(pink1, 0, mixer27, 0);
AudioConnection patchCord141(sine1, 0, mixer27, 1);
AudioConnection patchCord142(mixer25, 0, mixer28, 0);
AudioConnection patchCord143(mixer26, 0, mixer28, 1);
AudioConnection patchCord144(mixer27, 0, mixer28, 2);
AudioConnection patchCord145(mixer28, 0, biquad15, 0);
AudioConnection patchCord146(biquad1, 0, mixer29, 0);
AudioConnection patchCord147(biquad2, 0, mixer29, 1);
AudioConnection patchCord148(biquad3, 0, mixer29, 2);
AudioConnection patchCord149(biquad4, 0, mixer29, 3);
AudioConnection patchCord150(biquad5, 0, mixer30, 0);
AudioConnection patchCord151(biquad6, 0, mixer30, 1);
AudioConnection patchCord152(biquad7, 0, mixer30, 2);
AudioConnection patchCord153(biquad8, 0, mixer30, 3);
AudioConnection patchCord154(pink1, 0, mixer31, 0);
AudioConnection patchCord155(sine1, 0, mixer31, 1);
AudioConnection patchCord156(mixer29, 0, mixer32, 0);
AudioConnection patchCord157(mixer30, 0, mixer32, 1);
AudioConnection patchCord158(mixer31, 0, mixer32, 2);
AudioConnection patchCord159(mixer32, 0, biquad16, 0);
// GUItool: end automatically generated code



InputChannel route_inputs[8] = {
	InputChannel(&peak1, &rms1, &biquad1),
	InputChannel(&peak2, &rms2, &biquad2),
	InputChannel(&peak3, &rms3, &biquad3),
	InputChannel(&peak4, &rms4, &biquad4),
	InputChannel(&peak5, &rms5, &biquad5),
	InputChannel(&peak6, &rms6, &biquad6),
	InputChannel(&peak7, &rms7, &biquad7),
	InputChannel(&peak8, &rms8, &biquad8),
};

OutputChannel route_outputs[8] = {
	OutputChannel(&peak9, &rms9, &biquad9, {&mixer1, &mixer2, &mixer3}),
	OutputChannel(&peak10, &rms10, &biquad10, {&mixer5, &mixer6, &mixer7}),
	OutputChannel(&peak11, &rms11, &biquad11, {&mixer9, &mixer10, &mixer11}),
	OutputChannel(&peak12, &rms12, &biquad12, {&mixer13, &mixer14, &mixer15}),
	OutputChannel(&peak13, &rms13, &biquad13, {&mixer17, &mixer18, &mixer19}),
	OutputChannel(&peak14, &rms14, &biquad14, {&mixer21, &mixer22, &mixer23}),
	OutputChannel(&peak15, &rms15, &biquad15, {&mixer25, &mixer26, &mixer27}),
	OutputChannel(&peak16, &rms16, &biquad16, {&mixer29, &mixer30, &mixer31}),
};