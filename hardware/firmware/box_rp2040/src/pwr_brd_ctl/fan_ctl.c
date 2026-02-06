#include <stdint.h>
#include <math.h>
#include <hardware/i2c.h>

#include "config.h"
#include "fan_ctl.h"
#include "fan_ctl_regs.h"
#include "pwr_brd_ctl.h"
#include "io/serial.h"

uint16_t desired_fan_speed[NUMFAN];
uint16_t fan_calibrated[NUMFAN][FAN_MAX_PWM+1];
unsigned int fan_state = 0;
bool fan_error[NUMFAN];

void fan_ctl_init() {
    uint64_t now = time_us_64();

    for (int i = 0; i < NUMFAN; i++) {
        desired_fan_speed[i] = DESIRED_RPM;
        fan_error[i] = false;

        for (int j = 0; j < FAN_MAX_PWM; j++) {
            fan_calibrated[i][j] = 0;
        }
    }

    // Turn off the second fan by default, having both fans on makes it very noisy
    desired_fan_speed[1] = 0;
}

bool fan_ctl_i2c_read(uint8_t reg_id, uint8_t* dest) {
    return pwr_brd_i2c_read_reg(PWR_BRD_FAN_CTL_ADDR, reg_id, dest, 1);
}

bool fan_ctl_i2c_write(uint8_t reg_id, uint8_t val) {
    return pwr_brd_i2c_write_reg(PWR_BRD_FAN_CTL_ADDR, reg_id, &val, 1);
}

bool fan_ctl_set_pwm_frequency(uint8_t fan1, uint8_t fan2, uint8_t fan3, uint8_t fan4, uint8_t fan5) {
    uint8_t f123 = fan1 | (fan2<<2) | (fan3<<4);
    bool ret = fan_ctl_i2c_write(0x2D, f123);
    if (!ret) {
        return ret;
    }

    uint8_t f45 = fan4 | (fan5<<2);
    return fan_ctl_i2c_write(0x2C, f45);
}

void pwr_brd_fan_init() {
    uint64_t now = time_us_64();

    fan_ctl_set_pwm_frequency(0, 0, 0, 0, 0);
    for (int i = 0; i < NUMFAN; i++) {
        desired_fan_speed[i] = DESIRED_RPM;
    }
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

uint8_t fan_get_stall_speed(uint8_t fan_id) {
    for (int i = FAN_MAX_PWM; i > 0; i-- ) {
        if (fan_calibrated[fan_id][i] == 0) {
            return i;
        }
    }
}

void fan_ctl_task_calibrate() {
    static uint64_t fancal_last = 0;
    static uint8_t fancal_step = FAN_MAX_PWM;
    uint64_t now = time_us_64();
    if (now < (fancal_last + 2000000)) {
        return;
    }

    io_say_f("Calibration step %d\r\n", fancal_step);
    // First iteration doesn't store the values since the fans still
    // have to spin up
    if (fancal_last > 0) {
        uint16_t fanspeed;
        for(int i = 0; i < NUMFAN; i++) {
            fan_ctl_get_fan_speed(i, &fanspeed);
            fan_calibrated[i][fancal_step] = fanspeed;

            if (fancal_step == FAN_MAX_PWM && fanspeed == 0) {
                fan_error[i] = true;
            }
        }
        fancal_step--;
        if (fancal_step == 0) {
            // End calibration and move to the next state
            fan_state = FAN_STATE_RUN;
            /*
            io_say_f("Calibration finished\r\n");
            for (int fan =0; fan <NUMFAN;fan++){
                io_say_f("FAN %d: ", fan);
                if (fan_error[fan]) {
                    io_say_f("disconnected");
                } else {
                    for(int step=0;step<FAN_MAX_PWM;step++){
                        io_say_f("%d, ", fan_calibrated[fan][step]);
                    }
                }
                io_say_f("\r\n");
            }
            */
            return;
        }
    }

    for (int i = 0; i < NUMFAN; i++) {
        if(!fan_ctl_set_pwm(i, fancal_step)) {
            // io_say_f("Failed to set PWM on fan %d\r\n", i);
        }
    }
    fancal_last = time_us_64();
}

void fan_ctl_task_run() {
    uint64_t now = time_us_64();
    static uint64_t time_last = 0;
    if (now < (time_last + CMD_WAIT_TIME_US)) {
        return;
    }

    for (int i = 0; i < NUMFAN; i++ ) {
        if (fan_error[i]) {
            continue;
        }
        uint16_t fanspeed;
        fan_ctl_get_fan_speed(i, &fanspeed);

        // Anti-stall
        if (desired_fan_speed[i] > 0 && fanspeed == 0) {
            // io_say_f("Stall on fan %d\r\n", i);
            fan_ctl_set_pwm(i, fan_get_stall_speed(i) + 2);
            continue;
        }

        // Apply requested fan speed relative to stall speed
        uint8_t new_rpm = 0;
        if (desired_fan_speed[i] > 0) {
            new_rpm = fan_get_stall_speed(i) + desired_fan_speed[i];
        }
        fan_ctl_set_pwm(i, new_rpm);
    }
    time_last = time_us_64();
}

void fan_ctl_task() {
    switch (fan_state) {
        case FAN_STATE_CALIBRATE:
            return fan_ctl_task_calibrate();
        case FAN_STATE_RUN:
            return fan_ctl_task_run();
        default:
            fan_state = FAN_STATE_CALIBRATE;
            break;
    }
}
