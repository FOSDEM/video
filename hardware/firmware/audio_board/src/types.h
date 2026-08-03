#pragma once

#include "config.h"
#include "channel_info.h"
#include <stdint.h>

struct AudioState {
		float matrix_dB[CHANNELS][BUSES];

		uint16_t mutes[BUSES];
		uint16_t phantoms;

		float channel_input_gains_dB[CHANNELS];
		float bus_volumes_dB[BUSES];
};

struct Levels {
		float smooth[CHANNELS + BUSES];
		float peak[CHANNELS + BUSES];
		float rms[CHANNELS + BUSES];
		bool  state[CHANNELS + BUSES];
};
