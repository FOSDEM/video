#pragma once

#include <stdint.h>
#include <stdbool.h>

#define FAN_TACH_CONSTANT (7864320)
#define CMD_WAIT_TIME_US (3000 * 1000)
#define DESIRED_RPM (1)
#define DESIRED_RPM_THRESH_UPPER (1000)
#define DESIRED_RPM_THRESH_LOWER (2000)
#define FAN_MIN_HEALTHY_RPM (3000)
#define FAN_MAX_HEALTHY_RPM (10000)
#define NUMFAN (5)
#define FAN_MAX_PWM (6)

#define FAN_STATE_CALIBRATE (0)
#define FAN_STATE_RUN (1)

bool fan_ctl_get_fan_speed(uint8_t fan_id, uint16_t* dest);
bool fan_ctl_get_fan_status(uint8_t* dest);
bool fan_ctl_get_pwm(uint8_t fan_id, uint8_t* dest);
bool fan_ctl_set_pwm(uint8_t fan_id, uint8_t duty);
bool fan_ctl_set_fan_speed(uint8_t fan_id, uint16_t speed);
bool fan_ctl_set_fan_speed_target(uint8_t fan_id, uint16_t speed);
void fan_ctl_init(void);
void fan_ctl_task(void);
