#ifndef TEENSY_AUDIO_TAA3040_H
#define TEENSY_AUDIO_TAA3040_H
#include <stdint.h>

#define REG_P0_SW_RESET (0x01)
#define REG_P0_SLEEP_CFG (0x02)
#define REG_P0_ASI_CFG0 (0x07)
#define REG_P0_ASI_CFG1 (0x08)
#define REG_P0_MST_CFG0 (0x13)
#define REG_P0_ASI_STS (0x15)
#define REG_P0_ASI_OUT_CH_EN (0x74)
#define REG_P0_PWR_CFG (0x75)
#define REG_P0_DEV_STS1 (0x77)
#define REG_P0_CH1_CFG0 (0x3C)
#define REG_P0_CH1_CFG1 (0x3D)
#define REG_P0_CH1_CFG2 (0x3E)
#define REG_P0_DSP_CFG0 (0x6B)
#define REG_P0_DSP_CFG1 (0x6C)
#define REG_P0_AGC_CFG0 (0x70)

#define IMPEDANCE_2k5 (0b00)
#define IMPEDANCE_10k (0b01)
#define IMPEDANCE_20k (0b10)

#define REG_PWR_CFG_ADC_PDZ (1<<6)
#define REG_PWR_CFG_PLL_PDZ (1<<5)
#define REG_PWR_CFG_MICBIAS_PDZ (1<<7)
#define REG_PWR_CFG_DYN_CH_PUPD_EN (1<<4)

#define INTYP_MICROPHONE (0)
#define INTYP_LINE (1)

#define TYPE_DIFFERENTIAL (0)
#define TYPE_SINGLE_ENDED (1)
#define TYPE_PDM (2)

#define COUPLE_AC (0)
#define COUPLE_DC (1)

#define DECIMATION_LINEAR_PHASE (0)
#define DECIMATION_LOW_LATENCY (1)
#define DECIMATION_ULTRA_LOW_LATENCY (2)

#define HPF_PROGRAMMABLE (0)
#define HPF_12Hz (1)
#define HPF_96Hz (2)
#define HPF_384Hz (3)

class AudioControlTAA3040 {
public:
    bool enable();
    bool disable();
    bool gain(uint8_t channel, uint8_t gain);
    bool inputConfig(uint8_t channel, uint8_t impedance, uint8_t mode,
                     uint8_t coupling, uint8_t type, bool agc);
    void inputVolume(uint8_t channel, uint8_t volume);
    void agcConfig(uint8_t target, uint8_t gainlimit);
    void agcEnable(bool enabled);
    bool dspConfig(uint8_t decimation_filter, uint8_t hpf);
    void getAsiStatus();

private:
    void setRegister(uint8_t reg, uint8_t value);
    void setRegister(uint8_t page, uint8_t reg, uint8_t value);
    uint8_t getRegister(uint8_t page, uint8_t reg);

    void configASI0(uint8_t format, uint8_t wlen, uint8_t fsync_pol,
                    uint8_t bclk_pol, uint8_t tx_edge, uint8_t tx_fill);

    uint8_t currentPage = 0;
    uint8_t last_sts1 = 0;
    uint8_t last_asi = 0;
};

#endif // TEENSY_AUDIO_TAA3040_H