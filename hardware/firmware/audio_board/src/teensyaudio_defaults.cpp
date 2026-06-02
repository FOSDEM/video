#define MIC (5.0f)

const PROGMEM float default_bus_volumes_dB[BUSES] = {
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
};

const PROGMEM float default_channel_input_gains_dB[CHANNELS] = {
	MIC, MIC, MIC, 1.0f, 1.0f, 1.0f
};
// see helpers/generate_mutes.py
const PROGMEM uint64_t default_mutes = 52361428992;

const PROGMEM uint16_t default_phantoms = 0;

const PROGMEM float default_matrix_dB[CHANNELS][BUSES] = {
	// outputs: OUT1, OUT2, HP1, HP2, USB1, USB2
	// IN1
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// IN2
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// IN3
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// PC
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// USB1
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// USB2
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
};
