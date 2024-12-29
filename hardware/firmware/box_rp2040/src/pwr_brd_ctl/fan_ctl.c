#include <stdint.h>

#include <hardware/i2c.h>

#include "config.h"
#include "fan_ctl.h"
#include "fan_ctl_regs.h"
#include "pwr_brd_ctl.h"

uint64_t time_next_cmd[NUMFAN];
uint16_t desired_fan_speed[NUMFAN];

void pwr_brd_fan_init() {
    uint64_t now = time_us_64();

    for (int i = 0; i < NUMFAN; i++) {
        time_next_cmd[i] = now;
        desired_fan_speed[i] = DESIRED_RPM;
    }
}

bool fan_ctl_i2c_read(uint8_t reg_id, uint8_t* dest) {
    return pwr_brd_i2c_read_reg(PWR_BRD_FAN_CTL_ADDR, reg_id, dest, 1);
}

bool fan_ctl_i2c_write(uint8_t reg_id, uint8_t val) {
    return pwr_brd_i2c_write_reg(PWR_BRD_FAN_CTL_ADDR, reg_id, &val, 1);
}

bool fan_ctl_i2c_write_and_check(uint8_t reg_id, uint8_t val) {
    if (!pwr_brd_i2c_write_reg(PWR_BRD_FAN_CTL_ADDR, reg_id, &val, 1)) {
        return false;
    }
    uint8_t rval;
    if (!pwr_brd_i2c_read_reg(PWR_BRD_FAN_CTL_ADDR, reg_id, &rval, 1)) {
        return false;
    }

    if (rval != val) {
        return false;
    }
    return true;
}

bool fan_ctl_get_fan_speed(uint8_t fan_id, uint16_t* dest) {
    uint8_t lsb, msb;
    if (!fan_ctl_i2c_read(EMC2301_REG_TACHREADMSB + 0x10*fan_id, &msb)) {
        return false;
    }
    if (!fan_ctl_i2c_read(EMC2301_REG_TACHREADLSB + 0x10*fan_id, &lsb)) {
        return false;
    }

    if (msb == 0xFF && lsb == 0xF0) {
        *dest = 0;
        return true;
    }

    uint16_t tacho = ((msb << 8) | lsb) >> 3;

    // To avoid doubles, the fan pole multiplier was multiplied by 2 to make it an integer.
    // Here, we divide it (and the -1 in the bracket) by 2 to bring it back to its proper value.
    // *dest = 60 * tachMinRPMMultiplier_ * TACHO_FREQUENCY * (tachFanPolesMultiplier_ - 2) / 2 / fanPoleCount_ / tachoCount;

    // Based on the FOSDEM fans, documentation and guesswork
    // rpm = 7864320 / tacho

    uint16_t rpm = FAN_TACH_CONSTANT / tacho;

    *dest = rpm;
    return true;
}

bool fan_ctl_get_pwm(uint8_t fan_id, uint8_t* dest) {
    return fan_ctl_i2c_read(EMC2301_REG_FANSETTING + 0x10*fan_id, dest);
}

bool fan_ctl_set_pwm(uint8_t fan_id, uint8_t duty) {
    uint8_t fanconfig1;
    if (!fan_ctl_i2c_read(EMC2301_REG_FANCONFIG1 + 0x10*fan_id, &fanconfig1)) {
        return false;
    }
    fanconfig1 &= ~(1 << 7);    // disable PID controller
    if (!fan_ctl_i2c_write_and_check(EMC2301_REG_FANCONFIG1 + 0x10*fan_id, fanconfig1)) {
        return false;
    }

    return fan_ctl_i2c_write_and_check(EMC2301_REG_FANSETTING + 0x10*fan_id, duty);
}

bool fan_ctl_set_fan_speed(uint8_t fan_id, uint16_t speed) {
    uint8_t fanconfig1;
    if (!fan_ctl_i2c_read(EMC2301_REG_FANCONFIG1 + 0x10*fan_id, &fanconfig1)) {
        return false;
    }
    fanconfig1 |= (1 << 7);    // enable PID controller
    if (!fan_ctl_i2c_write_and_check(EMC2301_REG_FANCONFIG1 + 0x10*fan_id, fanconfig1)) {
        return false;
    }

    uint8_t lsb = (speed << 3) && 0xf8;
    uint8_t msb = (speed >> 5) && 0xff;
    if (!fan_ctl_i2c_write(EMC2301_REG_TACHTARGETLSB + 0x10 * fan_id, lsb)) {
        return false;
    }
    if (!fan_ctl_i2c_write(EMC2301_REG_TACHTARGETMSB + 0x10 * fan_id, msb)) {
        return false;
    }
    return true;
}

bool fan_ctl_set_fan_speed_target(uint8_t fan_id, uint16_t speed) {
    if (speed > 12000)
        return false;

    desired_fan_speed[fan_id] = speed;
    return true;
}

bool fan_ctl_get_fan_status(uint8_t* dest) {
    return fan_ctl_i2c_read(EMC2301_REG_FANSTATUS, dest);
}

/*

Standard fan control:

- if fan should be off, turn it off, unless it's already turned off
- if the fan speed is within a threshold, do nothing
- if fan speed is 0 and desired fan speed is more than 0, go to max
- if fan speed is above needed by more than 2x, lower pwm = pwm / 2
- if fan speed is above needed, pwm = pwm - 1
- if fan speed is below needed, pwm = pwm + 1
- any change is given 1s to take effect

*/

void pwr_brd_fan_task() {
    uint64_t now = time_us_64();

    for (int i = 0; i < NUMFAN; i++) {
        if (now < time_next_cmd[i]) {
            continue;
        }

        time_next_cmd[i] = now + CMD_WAIT_TIME_US;

        uint16_t fanspeed;
        fan_ctl_get_fan_speed(i, &fanspeed);

        if (desired_fan_speed[i] == 0) {
            // fan should be off

            if (fanspeed > 0) {
                // turn off fan
                fan_ctl_set_pwm(i, 0);
            }
            // otherwise, fan is off, as requested

            continue;
        }

        // in all cases below, fan should be on

        if (
            fanspeed > desired_fan_speed[i] - DESIRED_RPM_THRESH_LOWER &&
            fanspeed < desired_fan_speed[i] + DESIRED_RPM_THRESH_UPPER
        ) {
            // fan is within threshold, do nothing
            continue;
        }

        uint8_t pwm;
        fan_ctl_get_pwm(i, &pwm);

        if (pwm > FAN_MAX_PWM) {
            // PWM was set by the fan controller's default power on value
            // bring it back down to the max value
            fan_ctl_set_pwm(i, FAN_MAX_PWM);
            continue;
        }

        if (fanspeed == 0) {
            // initial spin-up
            // set to full speed

            if (pwm >= FAN_MAX_PWM) {
                // fan is already at full speed
                // it could be defective or disconnected
                // nothing to do
                continue;
            }

            fan_ctl_set_pwm(i, FAN_MAX_PWM);
            continue;
        }

        if (fanspeed > desired_fan_speed[i] * 2) {
            // fan is way too fast, dividing PWM in half
            fan_ctl_set_pwm(i, pwm >> 1);
            continue;
        }

        if (fanspeed > desired_fan_speed[i] + DESIRED_RPM_THRESH_UPPER) {
            // fan is a bit too fast

            if (pwm < 1) {
                // fan is already at minimum PWN, nothing to do
                continue;
            }

            // decrease PWM by 1
            fan_ctl_set_pwm(i, pwm - 1);

            // give it more time to spin down
            time_next_cmd[i] = now + 2 * CMD_WAIT_TIME_US;
            continue;
        }

        if (fanspeed < desired_fan_speed[i] - DESIRED_RPM_THRESH_LOWER) {
            // fan is a bit too slow

            if (pwm >= FAN_MAX_PWM) {
                // fan is already at max PWM, nothing to do
                continue;
            }

            // increase PWM by 1
            fan_ctl_set_pwm(i, pwm + 1);
            continue;
        }
    }
}
