#include "channels.h"

static constexpr ChanInfo the_channel_info[] = {
	{CHAN_WHITE, CHAN_ORANGE,  "1", "IN1", 0, 0, 6, 0},
	{CHAN_WHITE, CHAN_ORANGE, "2", "IN2", 0, 0, 18,0},
	{CHAN_WHITE, CHAN_ORANGE, "3", "IN3", 0, 0, 30,0},
	{CHAN_YELLOW, CHAN_YELLOW, "P", "PC", 0, 0, 54,0},
	{CHAN_MAGENTA, CHAN_MAGENTA, "USB", "USB1", 1, 0, 6,80},
	{CHAN_MAGENTA, CHAN_MAGENTA,"USB", "USB2", 2, 0, 18,80},
	{CHAN_BLUE, CHAN_GREY, "AES", "AES", 1, 0, 42,80},
	{CHAN_BLUE, CHAN_GREY, "AES", "AES", 2, 0, 54,80},

	{CHAN_WHITE, CHAN_WHITE, "1", "OUT1", 0, 1, 6,0},
	{CHAN_WHITE, CHAN_WHITE,"2", "OUT2", 0, 1, 18,0},
	{CHAN_GREEN, CHAN_GREEN,"AFL", "HP1", 1, 1, 42,0},
	{CHAN_GREEN, CHAN_GREEN,"AFL", "HP2", 2, 1, 54,0},
	{CHAN_MAGENTA, CHAN_MAGENTA, "USB", "USB1", 1, 1, 6,80},
	{CHAN_MAGENTA, CHAN_MAGENTA, "USB", "USB2", 2, 1, 18,80},
	{CHAN_BLUE, CHAN_GREY, "AES", "AES", 1, 1, 42,80},
	{CHAN_BLUE, CHAN_GREY, "AES", "AES", 2, 1, 54,80},
};

const ChanInfo& channel_info(uint8_t chan_id) {
	return the_channel_info[chan_id];
}
