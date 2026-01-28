#define MIC (1.5f)

const PROGMEM float default_bus_multipliers[BUSES] = {1.0f, 1.0f, 1.0f,
                                                      1.0f, 1.0f, 1.0f};
const PROGMEM float default_channel_multipliers[CHANNELS] = {MIC,  MIC,  MIC,
                                                             1.0f, 1.0f, 1.0f};
// see helpers/generate_mutes.py
const PROGMEM uint64_t default_mutes = 52361428992;

const PROGMEM float default_gains[CHANNELS][BUSES] = {
    // outputs: OUT1, OUT2, HP1, HP2, USB1, USB2
    // IN1
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
    // IN2
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
    // IN3
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
    // PC
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
    // USB1
    {1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
    // USB2
    {1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f}};
