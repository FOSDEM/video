#include <Audio.h>
#include <Wire.h>

#include <OSCMessage.h>
#include <SLIPEncodedSerial.h>

#include "config.h"
#include "helpers.h"

#ifdef EEPROM

#include "storage.h"

#endif

#ifdef DISPLAY

#include "display.h"

#endif

ChanInfo channel_info[] = {
	{CHAN_WHITE,   "1",   "XLR 1",       0},
	{CHAN_WHITE,   "2",   "XLR 2",       0},
	{CHAN_WHITE,   "3",   "XLR 3",       0},
	{CHAN_YELLOW,  "P",   "PC",          0},
	{CHAN_MAGENTA, "USB", "USB L",       1},
	{CHAN_MAGENTA, "USB", "USB R",       2},

	{CHAN_WHITE,   "1",   "XLR 1",       0},
	{CHAN_WHITE,   "2",   "XLR 2",       0},
	{CHAN_GREEN,   "AFL", "Headphone L", 1},
	{CHAN_GREEN,   "AFL", "Headphone R", 2},
	{CHAN_MAGENTA, "USB", "USB L",       1},
	{CHAN_MAGENTA, "USB", "USB R",       2},
};

#ifdef DISPLAY
ST7735_t3 display = ST7735_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
#endif

SLIPEncodedUSBSerial slip(Serial);

// GUItool: begin automatically generated code
AudioInputI2SQuad i2s_quad1;      //xy=336,677
AudioSynthNoisePink pink1;          //xy=338,824
AudioSynthWaveformSine sine1;          //xy=340,784
AudioInputUSB usb1;           //xy=341,743
AudioAnalyzePeak peak5;          //xy=603,433
AudioAnalyzePeak peak1;          //xy=604,299
AudioAnalyzePeak peak2;          //xy=605,332
AudioAnalyzePeak peak3;          //xy=605,366
AudioAnalyzePeak peak6;          //xy=605,467
AudioAnalyzePeak peak4;          //xy=607,399
AudioAnalyzeRMS rms5;           //xy=618,1246
AudioAnalyzeRMS rms6;           //xy=620,1279
AudioAnalyzeRMS rms3;           //xy=621,1180
AudioAnalyzeRMS rms1;           //xy=622,1115
AudioAnalyzeRMS rms2;           //xy=622,1148
AudioAnalyzeRMS rms4;           //xy=622,1213
AudioMixer4 mixer1;         //xy=787,364
AudioMixer4 mixer4; //xy=788,502
AudioMixer4 mixer2;         //xy=789,428
AudioMixer4 mixer5; //xy=790,566
AudioMixer4 mixer7; //xy=791,640
AudioMixer4 mixer8; //xy=793,704
AudioMixer4 mixer10; //xy=795,773
AudioMixer4 mixer11; //xy=797,837
AudioMixer4 mixer13; //xy=799,913
AudioMixer4 mixer14; //xy=801,977
AudioMixer4 mixer16; //xy=803,1046
AudioMixer4 mixer17; //xy=805,1110
AudioMixer4 mixer3;         //xy=949,395
AudioMixer4 mixer6; //xy=951,536
AudioMixer4 mixer9; //xy=951,673
AudioMixer4 mixer12; //xy=955,803
AudioMixer4 mixer15; //xy=956,948
AudioMixer4 mixer18; //xy=957,1075
AudioOutputI2SQuad i2s_quad2;      //xy=1324,614
AudioOutputUSB usb2;           //xy=1331,997
AudioAnalyzeRMS rms7;           //xy=1335,1094
AudioAnalyzeRMS rms8;           //xy=1336,1126
AudioAnalyzeRMS rms11;          //xy=1336,1221
AudioAnalyzeRMS rms12;          //xy=1336,1254
AudioAnalyzeRMS rms9;           //xy=1337,1158
AudioAnalyzeRMS rms10;          //xy=1338,1190
AudioAnalyzePeak peak7;          //xy=1342,270
AudioAnalyzePeak peak9;          //xy=1342,335
AudioAnalyzePeak peak10;         //xy=1342,366
AudioAnalyzePeak peak8;          //xy=1343,303
AudioAnalyzePeak peak11;         //xy=1343,397
AudioAnalyzePeak peak12;         //xy=1343,428
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
	{&mixer1,  &mixer2},
	{&mixer4,  &mixer5},
	{&mixer7,  &mixer8},
	{&mixer10, &mixer11},
	{&mixer13, &mixer14},
	{&mixer16, &mixer17},
};

AudioAnalyzeRMS *ent_rms[12] = {
	&rms1,
	&rms2,
	&rms3,
	&rms4,
	&rms5,
	&rms6,
	&rms7,
	&rms8,
	&rms9,
	&rms10,
	&rms11,
	&rms12,
};


AudioAnalyzePeak *ent_peak[12] = {
	&peak1,
	&peak2,
	&peak3,
	&peak4,
	&peak5,
	&peak6,
	&peak7,
	&peak8,
	&peak9,
	&peak10,
	&peak11,
	&peak12,
};

bool echo = false;
bool send_meters = false;

float gains[CHANNELS][BUSES];
uint64_t mutes;

int
mute_mask(int channel, int bus)
{
	return 1 << (bus * BUSES + channel);
}

float
get_gain(int channel, int bus)
{
	return gains[bus][channel];
}

bool
is_muted(int channel, int bus)
{
	return mutes & mute_mask(channel, bus);
}
bool
is_nan(float f)
{
	return f != f;
}

// raw audio
void
set_crosspoint(int channel, int bus, float gain)
{
	matrix[bus][channel / 4]->gain(channel % 4, gain);
}

float
get_crosspoint(int channel, int bus)
{
	return matrix[bus][channel / 4]->getGain(channel % 4);
}

// checking if muted
void set_gain(int channel, int bus, int gain) {
	gains[bus][channel] = gain;

	if(is_muted(channel, bus))
		matrix[bus][channel / 4]->gain(channel % 4, 0);
	else
		matrix[bus][channel / 4]->gain(channel % 4, gain);

#ifdef EEPROM
	eeprom_save_gains(gains);
#endif
}

void
mute(int channel, int bus)
{
	mutes |= mute_mask(channel, bus); // side effect
	set_crosspoint(channel, bus, 0);

#ifdef EEPROM
	eeprom_save_mutes(&mutes);
#endif
}

void
unmute(int channel, int bus)
{
	mutes &= ~mute_mask(channel, bus); // side effect
	set_crosspoint(channel, bus, gains[bus][channel]);
#ifdef EEPROM
	eeprom_save_mutes(&mutes);
#endif
}

void
set_mix(int bus, float in1, float in2, float in3, float in4, float in5, float in6)
{
	matrix[bus][0]->gain(0, in1);
	matrix[bus][0]->gain(1, in2);
	matrix[bus][0]->gain(2, in3);
	matrix[bus][0]->gain(3, in4);
	matrix[bus][1]->gain(0, in5);
	matrix[bus][1]->gain(1, in6);
	matrix[bus][1]->gain(2, 0.0f);
	matrix[bus][1]->gain(3, 0.0f);
}

void
set_mix(int bus, float bus_gains[CHANNELS])
{
	set_mix(bus, bus_gains[0], bus_gains[1], bus_gains[2], bus_gains[3], bus_gains[4], bus_gains[5]);
}

const PROGMEM float default_gains[BUSES][CHANNELS] = {
	// room PA
	{0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
	// livestream
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
	// Headphones
	{1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
	// USB out
	{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f}
};


void
default_state()
{
	memcpy(gains, default_gains, sizeof(gains));
	mutes = 0;

#ifdef EEPROM
	eeprom_save_gains(gains);
	eeprom_save_mutes(&mutes);
#endif
}

void
reset_state()
{
#ifdef EEPROM
	eeprom_load_gains(gains);
	eeprom_load_mutes(&mutes);

	int i, j;
	for(i = 0; i < BUSES; ++i)
	{
		for(j = 0; j < CHANNELS; ++j)
			if(isnan(gains[i][j])) goto no_eeprom;
		set_mix(i, gains[i]);
	}

	return;
no_eeprom:
	default_state();
#else
	default_state();
#endif
}

void
onOscChannel(OSCMessage &msg, int patternOffset)
{
	char buf[12];
	char address[22];
	int channel = -1;
	int addr;
	int offset;

	// /ch/<num>
	for (int i = 0; i < 6; i++) {
		sprintf(buf, "/%d", i);
		offset = msg.match(buf, patternOffset);
		if (offset) {
			channel = i;
			addr = offset + patternOffset;
			break;
		}
	}
	if (channel < 0) return;

	if (msg.match("/config/name", addr) > 0) {
		if (msg.isString(0)) {
			msg.getString(0, channel_info[channel].desc, sizeof(channel_info[channel].desc));
		} else {
			snprintf(address, 22, "/ch/%d/config/name", channel);
			OSCMessage response(address);
			response.add(channel_info[channel].desc);
			slip.beginPacket();
			response.send(slip);
			slip.endPacket();
		}
		return;
	}

	// /ch/<num>/mix
	offset = msg.match("/mix", addr);
	addr += offset;
	if (offset < 1) return;

	// /ch/<num>/mix/<bus>/level
	int bus = -1;
	for (int i = 0; i < 6; i++) {
		sprintf(buf, "/%d/level", i);
		int offset = msg.match(buf, addr);
		if (offset) {
			bus = i;
			break;
		}
	}
	if (bus < 0) return;

	if (msg.isFloat(0)) {
		set_gain(channel, bus, msg.getFloat(0));
	} else {
		snprintf(address, 22, "/ch/%d/mix/%d/level", channel, bus);
		OSCMessage response(address);
		response.add(get_gain(channel, bus));
		slip.beginPacket();
		response.send(slip);
		slip.endPacket();
	}
}

void
onOscBus(OSCMessage &msg, int patternOffset)
{
	char buf[12];
	char address[22];
	int bus = -1;
	int addr;
	int offset;

	// /bus/<num>
	for (int i = 0; i < 6; i++) {
		sprintf(buf, "/%d", i);
		offset = msg.match(buf, patternOffset);
		if (offset) {
			bus = i;
			addr = offset + patternOffset;
			break;
		}
	}
	if (bus < 0) return;

	if (msg.match("/config/name", addr) > 0) {
		if (msg.isString(0)) {
			msg.getString(0, channel_info[bus + 6].desc, sizeof(channel_info[bus + 6].desc));
		} else {
			snprintf(address, 22, "/bus/%d/config/name", bus);
			OSCMessage response(address);
			response.add(channel_info[bus + 6].desc);
			slip.beginPacket();
			response.send(slip);
			slip.endPacket();
		}
		return;
	}
}


void
onPacketReceived(OSCMessage msg)
{
	msg.route("/ch", onOscChannel);
	msg.route("/bus", onOscBus);
}

void
setup()
{
#ifdef DISPLAY
	display_setup(&display);
#endif

	AudioMemory(64);
	reset_state();

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

	slip.begin(115200);
}

float levels_rms[CHANNELS + BUSES];
float levels_smooth[CHANNELS + BUSES];
float levels_peak[CHANNELS + BUSES];

unsigned long last_draw = 0;

void
loop()
{
	int size;
	OSCMessage msg;
	if (slip.available()) {
		while (!slip.endofPacket()) {
			if ((size = slip.available()) > 0) {
				while (size--)
					msg.fill(slip.read());
			}
		}
		if (!msg.hasError()) {
			onPacketReceived(msg);
		}
	}

	float temp;
	if (rms1.available()) {
		for (int i = 0; i < 12; i++) {
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

#ifdef DISPLAY
		update_display(&display, levels_rms, channel_info);

		if (last_draw < (millis() - 16)) {
			display.updateScreen();
			last_draw = millis();
		}
#endif
	}

}

int
main()
{
	setup();
	while (1) {
		loop();
	}
};
