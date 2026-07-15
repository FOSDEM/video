#define MIC (5.0f)

const PROGMEM float default_bus_volumes_dB[BUSES] = {
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
};

const PROGMEM float default_channel_input_gains_dB[CHANNELS] = {
	MIC, MIC, MIC, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
};
// see helpers/generate_mutes.py
const PROGMEM uint16_t default_mutes[BUSES] = {
	//          AAUU PIII
	//          EESS CNNN
	//          SSBB  321
	//          2121
	0b1111'1111'1010'0010, // OUT1
	0b1111'1111'0101'0101, // OUT2
	0b1111'1111'1010'0010, // HP1
	0b1111'1111'0101'0101, // HP2
	0b1111'1111'1010'0010, // USB1
	0b1111'1111'0101'0001, // USB2
	0b1111'1111'1010'0010, // AES1
	0b1111'1111'0101'0001, // AES2
};

const PROGMEM uint16_t default_phantoms = 0;

const PROGMEM float default_matrix_dB[CHANNELS][BUSES] = {
	// outputs: OUT1, OUT2, HP1, HP2, USB1, USB2
	// IN1
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// IN2
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// IN3
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// PC
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// USB1
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// USB2
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// AES1
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
	// AES2
	{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
};
