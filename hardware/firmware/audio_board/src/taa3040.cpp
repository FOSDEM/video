
#include "taa3040.h"
#include "debug.h"
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

/*
 * Set the gain in the PGA in steps of 1dB. The gain range is from 0 to 42 dB. This also controls the
 * analog frontend configuration.
 */
bool AudioControlTAA3040::gain(uint8_t channel, uint8_t gain, uint8_t impedance, uint8_t mode, uint8_t coupling) {
	uint8_t offset = channel * 5;
	setRegister(REG_P0_CH1_CFG0 + offset, (impedance << 2) | (mode << 5) | (coupling << 4));
	setRegister(REG_P0_CH1_CFG1 + offset, gain << 2);
	return true;
}

/*
 * Set the digital volume control for the channel. This ranges from -100dB to +27dB with a
 * precision of 0.1 dB.
 * Note that this function is untested, unused and might have problems.
 * Why would you need in-ADC digital gain anyway?
 */
bool AudioControlTAA3040::digitalGain(uint8_t channel, float gainDb) {
	uint8_t offset  = channel * 5;
	float   halfDbs = gainDb * 2;

	uint8_t steps = (uint8_t)(halfDbs + 0.5);
	// 1.3 -> 2.6 -> 3 (1.5dB)
	uint8_t remain = (uint8_t)(halfDbs - (float)steps * 5);
	// 3 - 2.6 == 0.4 -> 2 0.1dB steps -> 1.5dB - 0.2dB == 1.3dB

	// The DigitalVolume does large adjustments in 0.5dB steps
	uint8_t val = 201 + steps;
	setRegister(REG_P0_CH1_CFG2 + offset, val);

	// Use the Calibration register to do fine adjustments in 0.1dB steps
	val = 8 + remain;
	setRegister(REG_P0_CH1_CFG3 + offset, val << 4);

	return true;
}

void AudioControlTAA3040::getAsiStatus() {
	uint8_t raw = getRegister(0, REG_P0_ASI_STS);
	if (raw == last_asi) {
		return;
	}
	last_asi      = raw;
	uint8_t ratio = raw & 0x0F;
	uint8_t rate  = raw >> 4;
	debug_print("ASI Status: ");
	switch (ratio) {
	case 0:
		debug_print("ratio 16, ");
		break;
	case 1:
		debug_print("ratio 24, ");
		break;
	case 2:
		debug_print("ratio 32, ");
		break;
	case 3:
		debug_print("ratio 48, ");
		break;
	case 4:
		debug_print("ratio 64, ");
		break;
	case 5:
		debug_print("ratio 96, ");
		break;
	case 6:
		debug_print("ratio 128, ");
		break;
	case 7:
		debug_print("ratio 192, ");
		break;
	case 8:
		debug_print("ratio 256, ");
		break;
	case 9:
		debug_print("ratio 384, ");
		break;
	case 10:
		debug_print("ratio 512, ");
		break;
	case 11:
		debug_print("ratio 1024, ");
		break;
	case 12:
		debug_print("ratio 2048, ");
		break;
	case 13:
	case 14:
		debug_printf("ratio RESERVED(%d), ", ratio);
		break;
	case 15:
		debug_print("ratio INVALID, ");
		break;
	}
	switch (rate) {
	case 0:
		debug_println("rate 7.35-8Khz");
		break;
	case 1:
		debug_println("rate 14.7-16Khz");
		break;
	case 2:
		debug_println("rate 22.05-24Khz");
		break;
	case 3:
		debug_println("rate 29.4-32Khz");
		break;
	case 4:
		debug_println("rate 44.1-48Khz");
		break;
	case 5:
		debug_println("rate 88.2-96Khz");
		break;
	case 6:
		debug_println("rate 176.4-192Khz");
		break;
	case 7:
		debug_println("rate 352.8-384Khz");
		break;
	case 8:
		debug_println("rate 705.6-768Khz");
		break;
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		debug_printf("rate RESERVED(%d)\n", rate);
		break;
	case 15:
		debug_println("rate INVALID");
		break;
	}
}

void AudioControlTAA3040::setRegister(uint8_t reg, uint8_t value) {
	Wire1.beginTransmission(0x4e);
	Wire1.write(reg);
	Wire1.write(value);
	if (Wire1.endTransmission() != 0) {
		debug_println("I2C error");
	}
}

void AudioControlTAA3040::setRegister(uint8_t page, uint8_t reg, uint8_t value) {
	debug_printf("REG %02x:%02x = %02x\n", page, reg, value);

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
		debug_println("I2C error");
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
