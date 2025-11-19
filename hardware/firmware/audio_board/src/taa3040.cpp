
#include "taa3040.h"
#include <Wire.h>

bool AudioControlTAA3040::enable(void) {
    setRegister(0, REG_P0_SW_RESET, 1);
    delay(100);
    setRegister(0, REG_P0_SW_RESET, 0);
    delay(100);
    setRegister(0, REG_P0_SLEEP_CFG, bit(0) | bit(7)); // Wake from sleep, use internal AREG generator
    delay(10);
    setRegister(0, REG_P0_ASI_OUT_CH_EN, 0xF0); // Enable ASI output channel 0-3
    configASI0(0, 0, 0, 0, 0, 1);

    setRegister(0, REG_P0_PWR_CFG, 0xE0); // Power on PLL and ADC
    delay(10);
    setRegister(0, REG_P0_MST_CFG0, bit(3)); // 44.1Khz mode, TDM slave
    return true;
}

bool AudioControlTAA3040::disable(void) {
    return true;
}

bool AudioControlTAA3040::gain(uint8_t channel, uint8_t gain, uint8_t impedance, uint8_t mode, uint8_t coupling) {
    uint8_t offset = channel * 5;
    setRegister(REG_P0_CH1_CFG0+offset, (impedance << 2)|(mode << 5)|(coupling<<4));
    setRegister(REG_P0_CH1_CFG1+offset, gain << 2);
    return true;
}

void AudioControlTAA3040::getAsiStatus() {
    uint8_t raw = getRegister(0, REG_P0_ASI_STS);
    if (raw == last_asi) {
        return;
    }
    last_asi = raw;
    uint8_t ratio = raw & 0x0F;
    uint8_t rate = raw >> 4;
    Serial.print("ASI Status: ");
    switch(ratio) {
    case 0:
        Serial.print("ratio 16, ");
        break;
    case 1:
        Serial.print("ratio 24, ");
        break;
    case 2:
        Serial.print("ratio 32, ");
        break;
    case 3:
        Serial.print("ratio 48, ");
        break;
    case 4:
        Serial.print("ratio 64, ");
        break;
    case 5:
        Serial.print("ratio 96, ");
        break;
    case 6:
        Serial.print("ratio 128, ");
        break;
    case 7:
        Serial.print("ratio 192, ");
        break;
    case 8:
        Serial.print("ratio 256, ");
        break;
    case 9:
        Serial.print("ratio 384, ");
        break;
    case 10:
        Serial.print("ratio 512, ");
        break;
    case 11:
        Serial.print("ratio 1024, ");
        break;
    case 12:
        Serial.print("ratio 2048, ");
        break;
    case 13:
    case 14:
        Serial.printf("ratio RESERVED(%d), ", ratio);
        break;
    case 15:
        Serial.print("ratio INVALID, ");
        break;
    }
    switch(rate) {
    case 0:
        Serial.println("rate 7.35-8Khz");
        break;
    case 1:
        Serial.println("rate 14.7-16Khz");
        break;
    case 2:
        Serial.println("rate 22.05-24Khz");
        break;
    case 3:
        Serial.println("rate 29.4-32Khz");
        break;
    case 4:
        Serial.println("rate 44.1-48Khz");
        break;
    case 5:
        Serial.println("rate 88.2-96Khz");
        break;
    case 6:
        Serial.println("rate 176.4-192Khz");
        break;
    case 7:
        Serial.println("rate 352.8-384Khz");
        break;
    case 8:
        Serial.println("rate 705.6-768Khz");
        break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        Serial.printf("rate RESERVED(%d)\n", rate);
        break;
    case 15:
        Serial.println("rate INVALID");
        break;
    }

}

void AudioControlTAA3040::setRegister(uint8_t reg, uint8_t value) {
    Wire1.beginTransmission(0x4e);
    Wire1.write(reg);
    Wire1.write(value);
    if (Wire1.endTransmission() != 0) {
        Serial.println("I2C error");
    }
}

void AudioControlTAA3040::setRegister(uint8_t page, uint8_t reg, uint8_t value) {
    Serial.print("REG ");
    Serial.print(page, HEX);
    Serial.print(":");
    Serial.print(reg, HEX);
    Serial.print(" = ");
    Serial.println(value, HEX);

    if (page != currentPage) {
        setRegister(0, page);
        currentPage = page;
    }
    setRegister(reg, value);
}

uint8_t AudioControlTAA3040::getRegister(uint8_t page, uint8_t reg) {
    if (page != currentPage) {
        setRegister(0, page);
        currentPage = page;
    }
    Wire1.beginTransmission(0x4e);
    Wire1.write(reg);
    Wire1.endTransmission(false);
    Wire1.requestFrom(0x4e, 1, true);
    const uint8_t res = Wire.read();
    if (Wire1.endTransmission() != 0) {
        Serial.println("I2C error");
    }
    return res;
}

void AudioControlTAA3040::configASI0(uint8_t format, uint8_t wlen, uint8_t fsync_pol, uint8_t bclk_pol, uint8_t tx_edge, uint8_t tx_fill) {
    uint8_t val = tx_fill;
    val |= (tx_edge & 1) << 1;
    val |= (bclk_pol & 1) << 2;
    val |= (fsync_pol & 1) << 3;
    val |= (wlen & 3) << 4;
    val |= (format & 3) << 6;
    setRegister(0, REG_P0_ASI_CFG0, val);
}