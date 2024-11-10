#include <stdint.h>

#include <hardware/i2c.h>

#include "config.h"
#include "fan_ctl.h"
#include "fan_ctl_regs.h"
#include "io/serial.h"
#include "pwr_brd_ctl.h"

#define FAN_CTL_I2C_TIMEOUT 200000

uint64_t time_last_cmd[NUMFAN];
uint16_t desired_fan_speed[NUMFAN];

void pwr_brd_fan_init() {
    for (int i = 0; i < NUMFAN; i++) {
        time_last_cmd[i] = 0;
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

#define FAN_TACH_CONSTANT (7864320)

bool fan_ctl_get_fan_speed(uint8_t fan_id, uint16_t* dest) {
    uint8_t lsb, msb;
    if (!fan_ctl_i2c_read(EMC2301_REG_TACHREADMSB + 0x10*fan_id, &msb)) {
        return false;
    }
    if (!fan_ctl_i2c_read(EMC2301_REG_TACHREADLSB + 0x10*fan_id, &lsb)) {
        return false;
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
    if (speed < 100 || speed > 12000)
        return false;

    desired_fan_speed[fan_id] = speed;
    return true;
}

bool fan_ctl_get_fan_status(uint8_t* dest) {
    return fan_ctl_i2c_read(EMC2301_REG_FANSTATUS, dest);
}


#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

/*

Standard fan control:

- if fan speed is 0, go to max
- if fan speed is above needed by more than 2x, lower pwm = pwm / 2
- if fan speed is above needed by less than 2x, lower pwm = pwm - 1
- if fan speed is below needed, pwm = pwm + 1
- any change is given 1s to take effect
- the fan speed is within a threshold, so if we are at less than desired + threshold and above desired - threshold, we do nothing

*/

void pwr_brd_fan_task() {
    static uint16_t cnt=0;
    int i;
    uint64_t now;

    cnt++;

    if (likely(cnt % FAN_WAIT_LOOPS != 0))
        return;

    now = time_us_64();
    for (i = 0; i < NUMFAN; i++) {
        if (time_last_cmd[i] + CMD_WAIT_TIME_US < now) continue;

        {
            uint16_t fanspeed;
            uint8_t pwm;

            fan_ctl_get_fan_speed(i, &fanspeed);
            if (fanspeed > desired_fan_speed[i] - DESIRED_RPM_THRESH && fanspeed < desired_fan_speed[i] + DESIRED_RPM_THRESH) {
                continue;
            } else if (fanspeed == 0) {
                fan_ctl_set_pwm(i, 255);
            } else if (fanspeed > desired_fan_speed[i] * 2) {
                fan_ctl_get_pwm(i, &pwm);
                fan_ctl_set_pwm(i, pwm / 2 );
            } else if (fanspeed > desired_fan_speed[i] + DESIRED_RPM_THRESH) {
                fan_ctl_get_pwm(i, &pwm);
                fan_ctl_set_pwm(i, pwm - 1 );
            } else if (fanspeed < desired_fan_speed[i] - DESIRED_RPM_THRESH) {
                fan_ctl_get_pwm(i, &pwm);
                fan_ctl_set_pwm(i, pwm + 1 );
            }
            time_last_cmd[i] = now;
        }
    }

}
