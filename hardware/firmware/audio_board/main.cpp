#include <Audio.h>
#include <Wire.h>

#include <OSCMessage.h>
#include <SLIPEncodedSerial.h>

#include "config.h"
#include "helpers.h"

#include "teensyaudio.h"

#ifdef EEPROM

#include "storage.h"

#endif

#ifdef DISPLAY

#include "display.h"

#endif

ChanInfo channel_info[] = {
	{CHAN_WHITE,   "1",   "IN1",        0},
	{CHAN_WHITE,   "2",   "IN2",        0},
	{CHAN_WHITE,   "3",   "IN3",        0},
	{CHAN_YELLOW,  "P",   "PC",         0},
	{CHAN_MAGENTA, "USB", "USB1",       1},
	{CHAN_MAGENTA, "USB", "USB2",       2},

	{CHAN_WHITE,   "1",   "OUT1",       0},
	{CHAN_WHITE,   "2",   "OUT2",       0},
	{CHAN_GREEN,   "AFL", "HP1",        1},
	{CHAN_GREEN,   "AFL", "HP2",        2},
	{CHAN_MAGENTA, "USB", "USB1",       1},
	{CHAN_MAGENTA, "USB", "USB2",       2},
};

#ifdef DISPLAY
ST7735_t3 display = ST7735_t3(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
#endif

SLIPEncodedUSBSerial slip(Serial);

bool echo = false;
bool send_meters = false;

float gains[CHANNELS][BUSES];
uint64_t mutes;

float channel_multipliers[CHANNELS];
float bus_multipliers[BUSES];

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

// checking if muted
void set_gain(int channel, int bus, int gain) {
	gains[bus][channel] = gain;

	raw_set_crosspoint(channel, bus, gain * !is_muted(channel, bus) * bus_multipliers[bus] * channel_multipliers[channel]);

#ifdef EEPROM
	eeprom_save_gains(gains);
#endif
}

void
mute(int channel, int bus)
{
	mutes |= mute_mask(channel, bus); // side effect
	set_gain(channel, bus, gains[bus][channel]);

#ifdef EEPROM
	eeprom_save_mutes(&mutes);
#endif
}

void
unmute(int channel, int bus)
{
	mutes &= ~mute_mask(channel, bus); // side effect
	set_gain(channel, bus, gains[bus][channel]);

#ifdef EEPROM
	eeprom_save_mutes(&mutes);
#endif
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
	int i;

	memcpy(gains, default_gains, sizeof(gains));
	mutes = 0;
	for(i = 0; i < BUSES; ++i)
		bus_multipliers[i] = 1.0f;
	for(i = 0; i < CHANNELS; ++i)
		channel_multipliers[i] = 1.0f;

#ifdef EEPROM
	eeprom_save_all(gains, &mutes, bus_multipliers, channel_multipliers);
#endif
}

void
reset_state()
{
#ifdef EEPROM
	eeprom_load_all(gains, &mutes, bus_multipliers, channel_multipliers);

	int i, j;
	for(i = 0; i < BUSES; ++i)
	{
		for(j = 0; j < CHANNELS; ++j)
			if(isnan(gains[i][j])) goto no_eeprom;
		raw_set_mix(i, gains[i]);
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
	reset_state();

	audio_setup();

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

	update_levels(levels_smooth, levels_rms, levels_peak);

#ifdef DISPLAY
	update_display(&display, levels_rms, channel_info);

	if (last_draw < (millis() - 16)) {
		display.updateScreen();
		last_draw = millis();
	}
#endif
}

int
main()
{
	setup();
	while (1) {
		loop();
	}
};
