#include "cli.h"

#include "../config.h"
#include "../teensyaudio.h"
#include "../db_conversion.h"
#include "../debug.h"
#include "../channels.h"

const Cli::CmdDescr Cli::cmds[Cli::num_cmds + 1] = {
	{
		.name     = "ping",
		.help     = "returns `pong <text>`",
		.arghelp  = "<text>",
		.num_args = -1,
		.callback = [](Cli* cli) {
			cli->port->printf("pong %s\n", cli->cmd);
		},
	},
	{
		.name     = "channel.labels",
		.help     = "returns a list of the short labels of inputs and buses",
		.arghelp  = "",
		.num_args = 0,
		.callback = [](Cli* cli) {
			cli->prefix_ok();
			cli->port->print("channels:");
			for (uint8_t i = 0; i < CHANNELS; i++) {
				cli->port->print(" ");
				cli->port->print(channel_info(i).label);
			}
			cli->port->print("; buses:");
			for (uint8_t i = 0; i < BUSES; i++) {
				cli->port->print(" ");
				cli->port->print(channel_info(CHANNELS + i).label);
			}
			cli->port->println();
		},
	},
	{
		.name     = "channel.names",
		.help     = "returns a list of the names of inputs and buses",
		.arghelp  = "",
		.num_args = 0,
		.callback = [](Cli* cli) {
			cli->prefix_ok();
			cli->port->print("channels:");
			for (uint8_t i = 0; i < CHANNELS; i++) {
				cli->port->print(" ");
				cli->port->print(channel_info(i).desc);
			}
			cli->port->print("; buses:");
			for (uint8_t i = 0; i < BUSES; i++) {
				cli->port->print(" ");
				cli->port->print(channel_info(CHANNELS + i).desc);
			}
			cli->port->println();
		},
	},
	{
		.name     = "levels.db",
		.help     = "for each input channel and then for each output bus, returns 3 numbers that are the rms, peak and smooth levels in dBu",
		.arghelp  = "",
		.num_args = 0,
		.callback = [](Cli* cli) {
			Levels& levels = audio_get_levels();
			cli->prefix_ok();
			for (uint8_t i = 0; i < CHANNELS + BUSES; i++) {
				cli->port->print(" ");
				cli->print_float_fixed(out_level_to_dBu(levels.rms[i]), 3, 5);
				cli->port->print(" ");
				cli->print_float_fixed(out_level_to_dBu(levels.peak[i]), 3, 5);
				cli->port->print(" ");
				cli->print_float_fixed(out_level_to_dBu(levels.smooth[i]), 3, 5);
			}
			cli->port->println();
		},
	},
	{
		.name     = "levels",
		.help     = "for each input channel and then for each output bus, returns 3 numbers that are the rms, peak and smooth levels",
		.arghelp  = "",
		.num_args = 0,
		.callback = [](Cli* cli) {
			Levels& levels = audio_get_levels();
			cli->prefix_ok();
			for (uint8_t i = 0; i < CHANNELS + BUSES; i++) {
				cli->port->print(" ");
				cli->print_float_fixed(levels.rms[i], 3, 5);
				cli->port->print(" ");
				cli->print_float_fixed(levels.peak[i], 3, 5);
				cli->port->print(" ");
				cli->print_float_fixed(levels.smooth[i], 3, 5);
			}
			cli->port->println();
		},
	},
	{
		.name     = "matrix",
		.help     = "for each input channel x outpub bus combination, outputs an item in the form <send>*<volume>, where send is 0/1 and volume is in decibels",
		.arghelp  = "",
		.num_args = 0,
		.callback = [](Cli* cli) {
			cli->prefix_ok();
			for (uint8_t chan = 0; chan < CHANNELS; chan++) {
				for (uint8_t bus = 0; bus < BUSES; bus++) {
					if (is_muted(chan, bus)) {
						cli->port->print(" 0*");
					} else {
						cli->port->print(" 1*");
					}
					cli->print_float_fixed(get_volume_dB(chan, bus), 3, 3);
				}
			}
			cli->port->println();
		},
	},
	{
		.name     = "gains",
		.help     = "for each input channel, returns its gain in db",
		.arghelp  = "",
		.num_args = 0,
		.callback = [](Cli* cli) {
			cli->prefix_ok();
			for (uint8_t chan = 0; chan < CHANNELS; chan++) {
				cli->port->print(" ");
				cli->print_float_fixed(get_channel_input_gain_dB(chan), 3, 3);
			}
			cli->port->println();
		},
	},
	{
		.name     = "phantoms",
		.help     = "for each input channel, returns its phantom power on/off status (0/1)",
		.arghelp  = "",
		.num_args = 0,
		.callback = [](Cli* cli) {
			cli->prefix_ok();
			for (uint8_t chan = 0; chan < CHANNELS; chan++) {
				if (is_phantom_on(chan)) {
					cli->port->print(" 1");
				} else {
					cli->port->print(" 0");
				}
			}
			cli->port->println();
		},
	},
	{
		.name     = "bus-volumes",
		.help     = "for each output bus, returns its volume in dB",
		.arghelp  = "",
		.num_args = 0,
		.callback = [](Cli* cli) {
			cli->prefix_ok();
			for (uint8_t bus = 0; bus < BUSES; bus++) {
				cli->port->print(" ");
				cli->print_float_fixed(get_bus_volume_dB(bus), 3, 3);
			}
			cli->port->println();
		},
	},
	{
		.name     = "send.set",
		.help     = "for the given channel/bus crosspoint, set the send bit in the matrix",
		.arghelp  = "<in channel no> <out bus no> (0|1)",
		.num_args = 3,
		.callback = [](Cli* cli) {
			uint16_t chan      = cli->hop_uint();
			uint16_t bus       = cli->hop_uint();
			uint16_t want_send = cli->hop_uint();
			if (chan >= CHANNELS) {
				cli->prefix_fail();
				cli->port->printf("chan %d is invalid\n", chan);
				return;
			}
			if (bus >= BUSES) {
				cli->prefix_fail();
				cli->port->printf("bus %d is invalid\n", bus);
				return;
			}
			if (want_send > 0) {
				unmute(chan, bus);
			} else {
				mute(chan, bus);
			}
			cli->report_ok();
		},
	},
	{
		.name     = "phantom.set",
		.help     = "for the given input channel, set the phantom power on/off bit",
		.arghelp  = "<in channel no> (0|1)",
		.num_args = 2,
		.callback = [](Cli* cli) {
			uint16_t chan         = cli->hop_uint();
			uint16_t want_phantom = cli->hop_uint();
			if (chan >= CHANNELS) {
				cli->prefix_fail();
				cli->port->printf("chan %d is invalid\n", chan);
				return;
			}
			if (want_phantom > 0) {
				set_phantom_on(chan);
			} else {
				set_phantom_off(chan);
			}
			cli->report_ok();
		},
	},
	{
		.name     = "volume.set",
		.help     = "for the given channel/bus crosspoint, set the volume in the matrix",
		.arghelp  = "<in channel no> <out bus no> <volume in dB>",
		.num_args = 3,
		.callback = [](Cli* cli) {
			uint16_t chan = cli->hop_uint();
			uint16_t bus  = cli->hop_uint();
			float    vol  = cli->hop_float();

			if (chan >= CHANNELS) {
				cli->prefix_fail();
				cli->port->printf("chan %d is invalid\n", chan);
				return;
			}
			if (bus >= BUSES) {
				cli->prefix_fail();
				cli->port->printf("bus %d is invalid\n", bus);
				return;
			}

			set_volume_dB(chan, bus, vol);

			cli->report_ok();
		},
	},
	{
		.name     = "in-gain.set",
		.help     = "for the given input channel, set the input gain in decibels",
		.arghelp  = "<in channel no> <gain>",
		.num_args = 2,
		.callback = [](Cli* cli) {
			uint16_t chan = cli->hop_uint();
			float    gain = cli->hop_float();

			if (chan >= CHANNELS) {
				cli->prefix_fail();
				cli->port->printf("chan %d is invalid\n", chan);
				return;
			}

			set_channel_input_gain_dB(chan, gain);

			cli->report_ok();
		},
	},
	{
		.name     = "bus-volume.set",
		.help     = "for the given out bus, set its global volume to the given value",
		.arghelp  = "<out bus no> <volume in dB>",
		.num_args = 2,
		.callback = [](Cli* cli) {
			uint16_t bus = cli->hop_uint();
			float    vol = cli->hop_float();

			if (bus >= BUSES) {
				cli->prefix_fail();
				cli->port->printf("bus %d is invalid\n", bus);
				return;
			}

			set_bus_volume_dB(bus, vol);

			cli->report_ok();
		},
	},
	{
	.name     = "channel.eq.set",
	.help     = "for the given channel, configure an eq band",
	.arghelp  = "<channel no> <band no> <shape> <freq> <gain> <q>",
	.num_args = 6,
	.callback = [](Cli* cli) {
		uint16_t chan = cli->hop_uint();
		uint16_t band = cli->hop_uint();
		uint16_t shape = cli->hop_uint();
		float freq = cli->hop_float();
		float    gain = cli->hop_float();
		float    q = cli->hop_float();

		if (chan >= CHANNELS) {
			cli->prefix_fail();
			cli->port->printf("channel %d is invalid [0-%d]\n", chan, CHANNELS-1);
			return;
		}
		if (band >= 4) {
			cli->prefix_fail();
			cli->port->printf("band %d is invalid [0-3]\n", chan);
			return;
		}

		switch (shape)
		{
		case 0:
			// All-pass
			get_channel(chan)->filter.SetAllpass(band);
			break;
		case 1:
			// Low-pass
			get_channel(chan)->filter.SetLowpass(band, freq, q);
			break;
		case 2:
			// High-pass
			get_channel(chan)->filter.SetHighpass(band, freq, q);
			break;
		default:
			cli->prefix_fail();
			cli->port->printf("invalid band shape %d\n", shape);
			return;
		}

		cli->report_ok();
	},
},
	{
		.name     = "factory-reset",
		.help     = "clear all settings and state stored in the EEPROM",
		.arghelp  = "",
		.num_args = 0,
		.callback = [](Cli* cli) {
			audio_reset_default_state();
			audio_eeprom_save_all();
			cli->report_ok();
		},
	},
	{
		.name     = "commands",
		.help     = "get a list of available commands",
		.arghelp  = "",
		.num_args = 0,
		.callback = [](Cli* cli) {
			cli->prefix_ok();
			for (uint8_t i = 0; i < Cli::num_cmds; i++) {
				cli->port->print(" ");
				cli->port->print(Cli::cmds[i].name);
			}
			cli->port->println();
		},
	},
	{
		.name     = "help",
		.help     = "I suppose you think that was terribly clever.",
		.arghelp  = "[command]",
		.num_args = -1,
		.callback = [](Cli* cli) {
			if (cli->cmd[0] == '\0') {
				cli->prefix_ok();
				cli->port->println("use `commands` to get a list of commands and then `help <command>`; when calling a command, prefix it with a number between 1 and 65535 to use as a slug that will be printed back with the response");
				return;
			}
			const Cli::CmdDescr* cmd = cli->hop_cmd();
			if (!cmd) {
				cli->prefix_fail();
				cli->port->println("unknown command; use `commands` for list of commands");
				return;
			}
			cli->prefix_ok();
			cli->print_usage(*cmd);
			cli->port->println();
		},
	},
	{
		.name     = "dbgtest",
		.help     = "print some log messages to check your debug logging",
		.arghelp  = "",
		.num_args = 0,
		.callback = [](Cli* cli) {
			for (uint8_t i = 0; i < 5; i++) {
				debug_print("foo ");
				debug_printf("bar %d\nbaz\n", i);
			}
		},
	},
	{
		.name = "END",
	},
};
