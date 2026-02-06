#include <OSCBundle.h>
#include <OSCMessage.h>
#include <SLIPEncodedSerial.h>

#include "config.h"
#include "helpers.h"
#include <stdint.h>

#include "teensyaudio.h"

#ifdef USE_EEPROM

#include "storage.h"

#endif

#ifdef USE_DISPLAY

#include "display.h"

#endif

ChanInfo channel_info[] = {
    {CHAN_WHITE, "1", "IN1", 0},      {CHAN_WHITE, "2", "IN2", 0},
    {CHAN_WHITE, "3", "IN3", 0},      {CHAN_YELLOW, "P", "PC", 0},
    {CHAN_MAGENTA, "USB", "USB1", 1}, {CHAN_MAGENTA, "USB", "USB2", 2},

    {CHAN_WHITE, "1", "OUT1", 0},     {CHAN_WHITE, "2", "OUT2", 0},
    {CHAN_GREEN, "AFL", "HP1", 1},    {CHAN_GREEN, "AFL", "HP2", 2},
    {CHAN_MAGENTA, "USB", "USB1", 1}, {CHAN_MAGENTA, "USB", "USB2", 2},
};

SLIPEncodedUSBSerial slip(Serial);

void send(OSCMessage &msg) {
    slip.beginPacket();
    msg.send(slip);
    slip.endPacket();
}
void send(OSCBundle &msg) {
    slip.beginPacket();
    msg.send(slip);
    slip.endPacket();
}

void onOscChannel(OSCMessage &msg, int patternOffset) {
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
    if (channel < 0)
        return;

    if (msg.match("/config/name", addr) > 0) {
        if (msg.isString(0)) {
            msg.getString(0, channel_info[channel].desc,
                          sizeof(channel_info[channel].desc));
            send(msg);
        } else {
            snprintf(address, 22, "/ch/%d/config/name", channel);
            OSCMessage response(address);
            response.add(channel_info[channel].desc);
            send(response);
        }
        return;
    } else if (msg.match("/levels", addr) > 0) {
        OSCBundle response;
        Levels &levels = audio_get_levels();

        snprintf(address, 22, "/ch/%d/levels/rms", channel);
        response.add(address).add(rmsToDb(levels.rms[channel]));

        snprintf(address, 22, "/ch/%d/levels/peak", channel);
        response.add(address).add(rmsToDb(levels.peak[channel]));

        snprintf(address, 22, "/ch/%d/levels/smooth", channel);
        response.add(address).add(rmsToDb(levels.smooth[channel]));

        send(response);
        return;
    } else if (msg.match("/multiplier", addr) > 0) {
        if (msg.isFloat(0)) {
            set_channel_multiplier(channel, msg.getFloat(0));
            send(msg);
        } else {
            snprintf(address, 22, "/ch/%d/multiplier", channel);
            OSCMessage response(address);
            response.add(get_channel_multiplier(channel));

            send(response);
        }
    }

    // /ch/<num>/mix
    offset = msg.match("/mix", addr);
    addr += offset;
    if (offset < 1)
        return;

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
    if (bus < 0)
        goto mutes;

    if (msg.isFloat(0)) {
        set_gain(channel, bus, msg.getFloat(0));
        send(msg);
    } else {
        snprintf(address, 22, "/ch/%d/mix/%d/level", channel, bus);
        OSCMessage response(address);
        response.add((float)get_gain(channel, bus));
        send(response);
    }

    return;

mutes:
    for (int i = 0; i < 6; i++) {
        sprintf(buf, "/%d/muted", i);
        int offset = msg.match(buf, addr);
        if (offset) {
            bus = i;
            break;
        }
    }
    if (bus < 0)
        return;

    if (msg.isBoolean(0)) {
        if (msg.getBoolean(0))
            mute(channel, bus);
        else
            unmute(channel, bus);
        send(msg);
    } else {
        snprintf(address, 22, "/ch/%d/mix/%d/muted", channel, bus);
        OSCMessage response(address);
        response.add(is_muted(channel, bus));
        send(response);
    }
}

void onOscInfo(OSCMessage &msg) {
    char addrbuf[22];
    OSCBundle info;

    int i;

    info.add("/info/buses").add((uint8_t)BUSES);
    info.add("/info/channels").add((uint8_t)CHANNELS);
    info.add("/info/features").add(FEATURES);

    for (i = 0; i < BUSES; ++i) {
        snprintf(addrbuf, 22, "/bus/%d/config/name", i);
        info.add(addrbuf).add(channel_info[CHANNELS + i].desc);
    }
    for (i = 0; i < CHANNELS; ++i) {
        snprintf(addrbuf, 22, "/ch/%d/config/name", i);
        info.add(addrbuf).add(channel_info[i].desc);
    }

    send(info);
}

void onOscFactoryReset(OSCMessage &msg) {
    SerialUSB1.println("factory reset requested");
    audio_reset_default_state();
    audio_eeprom_save_all();

    send(msg);
}

void onOscState(OSCMessage &msg, int patternOffset) {
    char addrbuf[31];
    OSCBundle info;

    int i, j;

    for (i = 0; i < CHANNELS; ++i) {
        for (j = 0; j < BUSES; ++j) {
            snprintf(addrbuf, 30, "/ch/%d/mix/%d/level", i, j);
            info.add(addrbuf).add(get_gain(i, j));
            snprintf(addrbuf, 30, "/ch/%d/mix/%d/muted", i, j);
            info.add(addrbuf).add(!!is_muted(i, j));
            snprintf(addrbuf, 30, "/ch/%d/mix/%d/raw", i, j);
            info.add(addrbuf).add(raw_get_crosspoint(i, j));
        }
    }

    for (i = 0; i < CHANNELS; ++i) {
        snprintf(addrbuf, 22, "/ch/%d/multiplier", i);
        info.add(addrbuf).add(get_channel_multiplier(i));
    }
    for (j = 0; j < BUSES; ++j) {
        snprintf(addrbuf, 22, "/bus/%d/multiplier", j);
        info.add(addrbuf).add(get_bus_multiplier(j));
    }

    send(info);
}

void onOscBus(OSCMessage &msg, int patternOffset) {
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
    if (bus < 0)
        return;

    if (msg.match("/config/name", addr) > 0) {
        if (msg.isString(0)) {
            msg.getString(0, channel_info[CHANNELS + bus].desc,
                          sizeof(channel_info[CHANNELS + bus].desc));
            send(msg);
        } else {
            snprintf(address, 22, "/bus/%d/config/name", bus);
            OSCMessage response(address);
            response.add(channel_info[CHANNELS + bus].desc);
            send(response);
        }
        return;
    } else if (msg.match("/levels", addr) > 0) {
        OSCBundle response;
        Levels &levels = audio_get_levels();

        snprintf(address, 22, "/bus/%d/levels/rms", bus);
        response.add(address).add(rmsToDb(levels.rms[CHANNELS + bus]));

        snprintf(address, 22, "/bus/%d/levels/peak", bus);
        response.add(address).add(rmsToDb(levels.peak[CHANNELS + bus]));

        snprintf(address, 22, "/bus/%d/levels/smooth", bus);
        response.add(address).add(rmsToDb(levels.smooth[CHANNELS + bus]));

        send(response);
    } else if (msg.match("/multiplier", addr) > 0) {
        if (msg.isFloat(0)) {
            set_bus_multiplier(bus, msg.getFloat(0));
            send(msg);
        } else {
            snprintf(address, 22, "/bus/%d/multiplier", bus);
            OSCMessage response(address);
            response.add(get_bus_multiplier(bus));
            send(response);
        }
    }
}

void onOscLevels(OSCMessage &msg, int patternOffset) {
    Levels &levels = audio_get_levels();
    OSCMessage response("/levels");

    float payload[(CHANNELS + BUSES) * 3 + 1];
    size_t offset = CHANNELS + BUSES;
    for (size_t i = 0; i < offset; i++) {
        payload[1 + i] = levels.rms[i];
        payload[1 + i + offset] = levels.peak[i];
        payload[1 + i + (offset * 2)] = levels.smooth[i];
    }
    uint8_t enc[sizeof(payload)] = {0};
    memcpy(&enc, &payload, sizeof(payload));
    enc[0] = (char)CHANNELS;
    enc[1] = (char)BUSES;
    enc[2] = 3;
    enc[3] = (unsigned char)0xFF;
    response.add(enc, sizeof(payload));

    send(response);
}

void onOscUnknown() {
    OSCMessage response("/unknown");
    send(response);
}

bool any(int start...) {
    bool found;
    if ((found = start))
        goto end;

    va_list args;
    va_start(args, start);

    while (!(found = va_arg(args, int)))
        ;

end:
    va_end(args);

    return found;
}

void onPacketReceived(OSCMessage msg) {
    // Note: all functions are invoked, so make sure there are no side-effects
    // if `false` is returned
    if (any(msg.route("/ch", onOscChannel), msg.route("/bus", onOscBus),
            msg.route("/state", onOscState), msg.dispatch("/info", onOscInfo),
            msg.dispatch("/factoryreset", onOscFactoryReset),
            msg.route("/levels", onOscLevels)))
        return;

    onOscUnknown();
}

void setup() {
    if (CrashReport) {
        // Wait until the debug interface is ready
        while (!SerialUSB1 && millis() < 1500)
            ;
        SerialUSB1.print(CrashReport);
    }

#ifdef USE_DISPLAY
    display_setup();
#endif
    audio_load_state();

    audio_setup();

    slip.begin(115200);
    SerialUSB1.println("board ready");
}

unsigned long last_draw = 0;
unsigned long last_save = 0;

void loop() {
    int size;
    OSCMessage msg;
    if (slip.available()) {
        while (!slip.endofPacket() && slip.available()) {
                while (1) {
		    int c = slip.read();
		    if (c >= 0 ) {
                        msg.fill(c);
			break;
		    }
		}
        }
        if (!msg.hasError()) {
            onPacketReceived(msg);
        }
    }

    Levels &levels = audio_get_levels();

    audio_update_levels(levels);

#ifdef USE_DISPLAY
    display_update_vu(levels.rms, channel_info);

    if (last_draw < (millis() - 16)) {
        display_update_screen();
        last_draw = millis();
    }
#endif

#ifdef USE_EEPROM
    // save to EEPROM every 60 seconds
    if (last_save + 60000 < millis()) {
        size = audio_eeprom_save_all();
        last_save = millis();

        SerialUSB1.printf("eeprom: wrote %d bytes\n", size);
    }
#endif
}

int main() {
    setup();
    while (1) {
        loop();
    }
};
