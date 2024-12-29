#include "pwr_brd_ctl.h"
#include "config.h"
#include "io/serial.h"
#include "tca9534.h"
#include <pico/stdlib.h>
#include <hardware/i2c.h>

expander_t expander;

void pwr_brd_ctl_init() {
    i2c_init(PWR_BRD_I2C_INST, PWR_BRD_I2C_BAUD);
    gpio_pull_up(PWR_BRD_I2C_SDA);
    gpio_pull_up(PWR_BRD_I2C_SCL);
    gpio_set_function(PWR_BRD_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PWR_BRD_I2C_SCL, GPIO_FUNC_I2C);
    expander_init(&expander, PWR_BRD_I2C_INST, PWR_BRD_EXPANDER_ADDR, PWR_BRD_I2C_TIMEOUT_US);
    pwr_brd_fan_init();
}

bool pwr_brd_raw_gpio_read(uint8_t* val) {
    return expander_read_inputs(&expander, val);
}

bool ensure_expander_output_dirs(void) {
    static bool is_setup = false;
    if (!is_setup) {
        if (!expander_set_pin_directions(&expander, PWR_BRD_EXPANDER_PIN_DIRS)) {
            return false;
        }
        is_setup = true;
    }
    return true;
}

bool pwr_brd_set_gpio_outs(uint8_t outs) {
    ensure_expander_output_dirs();
    return expander_write_outputs(&expander, outs);
}

bool pwr_brd_charger_power(bool on) {
    uint8_t output_state = expander.last_output_state;

    if (on) {
        output_state |= (1 << PWR_BRD_EXPANDER_PIN_CHG_PWR);
    } else {
        output_state &= ~(1 << PWR_BRD_EXPANDER_PIN_CHG_PWR);
    }

    return pwr_brd_set_gpio_outs(output_state);
}

bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

bool temp_sensor_get_temp_raw(uint8_t* dest) {
    return (i2c_read_blocking_until(PWR_BRD_I2C_INST, PWR_BRD_TEMP_SENS_ADDR, dest, 1, false, make_timeout_time_ms(20)) >= 0);
}

bool pwr_brd_i2c_read_reg(uint8_t dev_addr, uint8_t reg_id, uint8_t* dest, uint8_t num_bytes) {
    int ret = i2c_write_blocking_until(PWR_BRD_I2C_INST, dev_addr, &reg_id, 1, true, make_timeout_time_ms(20));
    if (ret != 1) {
        return false;
    }
    ret = i2c_read_blocking_until(PWR_BRD_I2C_INST, dev_addr, dest, num_bytes, false, make_timeout_time_ms(20));
    if (ret != num_bytes) {
        return false;
    }
    return true;
}

bool pwr_brd_i2c_write_reg(uint8_t dev_addr, uint8_t reg_id, uint8_t* src, uint8_t num_bytes) {
    uint8_t buf[9];
    if (num_bytes > 8) {
        return false;
    }
    for (uint8_t i = 0; i < num_bytes; i++) {
        buf[i + 1] = src[i];
    }
    buf[0] = reg_id;
    uint8_t ret = i2c_write_blocking_until(PWR_BRD_I2C_INST, dev_addr, buf, num_bytes + 1, false, make_timeout_time_ms(20));
    if (ret != num_bytes + 1) {
        io_say("wrote ");
        io_say_uint(ret);
        io_say(" instead of ");
        io_say_uint(num_bytes + 1);
        io_say("\n");
        return false;
    }
    return true;
}

void pwr_brd_i2c_bus_scan(bool found[NUM_I2C_ADDR]) {
    for (int addr = 0; addr < NUM_I2C_ADDR; ++addr) {
        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr)) {
            ret = PICO_ERROR_GENERIC;
        } else {
            ret = i2c_read_blocking_until(PWR_BRD_I2C_INST, addr, &rxdata, 1, false, make_timeout_time_ms(20));
        }

        found[addr] = ret >= 0;
    }
}

void pwr_brd_i2c_dump_all_regs(int16_t reg_values[NUM_I2C_REG], uint8_t addr) {
    for (uint8_t reg = 0; reg < NUM_I2C_REG; reg++) {
        uint8_t val;

        if (pwr_brd_i2c_read_reg(addr, reg, &val, 1)) {
            reg_values[reg] = val;
        } else {
            reg_values[reg] = PICO_ERROR_GENERIC;
        }
    }
}
