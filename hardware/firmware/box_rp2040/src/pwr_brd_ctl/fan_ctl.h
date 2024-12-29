#pragma once

#include <stdint.h>
#include <stdbool.h>

#define FAN_TACH_CONSTANT (7864320)
#define CMD_WAIT_TIME_US (1000*1000)
#define DESIRED_RPM (5000)
#define DESIRED_RPM_THRESH (1000)
#define NUMFAN (5)

bool fan_ctl_get_fan_speed(uint8_t fan_id, uint16_t* dest);
bool fan_ctl_get_fan_status(uint8_t* dest);
bool fan_ctl_get_pwm(uint8_t fan_id, uint8_t* dest);
bool fan_ctl_set_pwm(uint8_t fan_id, uint8_t duty);
bool fan_ctl_set_fan_speed(uint8_t fan_id, uint16_t speed);
bool fan_ctl_set_fan_speed_target(uint8_t fan_id, uint16_t speed);
