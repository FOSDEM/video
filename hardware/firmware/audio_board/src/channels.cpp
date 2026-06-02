#include "channels.h"

static constexpr ChanInfo the_channel_info[] = {
	{CHAN_WHITE, "1", "IN1", 0},
	{CHAN_WHITE, "2", "IN2", 0},
	{CHAN_WHITE, "3", "IN3", 0},
	{CHAN_YELLOW, "P", "PC", 0},
	{CHAN_MAGENTA, "USB", "USB1", 1},
	{CHAN_MAGENTA, "USB", "USB2", 2},

	{CHAN_WHITE, "1", "OUT1", 0},
	{CHAN_WHITE, "2", "OUT2", 0},
	{CHAN_GREEN, "AFL", "HP1", 1},
	{CHAN_GREEN, "AFL", "HP2", 2},
	{CHAN_MAGENTA, "USB", "USB1", 1},
	{CHAN_MAGENTA, "USB", "USB2", 2},
};

const ChanInfo& channel_info(uint8_t chan_id) {
	return the_channel_info[chan_id];
}
