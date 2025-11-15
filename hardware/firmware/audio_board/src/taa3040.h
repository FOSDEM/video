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

#define IMPEDANCE_2k5 (0b00)
#define IMPEDANCE_10k (0b01)
#define IMPEDANCE_20k (0b10)

class AudioControlTAA3040
{
  public:
    bool enable();
    bool disable();
    bool gain(uint8_t channel, uint8_t gain, uint8_t impedance);

    void getAsiStatus();

private:
    void setRegister(uint8_t reg, uint8_t value);
    void setRegister(uint8_t page, uint8_t reg, uint8_t value);
    uint8_t getRegister(uint8_t page, uint8_t reg);

    void configASI0(uint8_t format, uint8_t wlen, uint8_t fsync_pol, uint8_t bclk_pol, uint8_t tx_edge, uint8_t tx_fill);

    uint8_t currentPage = 0;
    uint8_t last_sts1 = 0;
    uint8_t last_asi = 0;
};

#endif // TEENSY_AUDIO_TAA3040_H
