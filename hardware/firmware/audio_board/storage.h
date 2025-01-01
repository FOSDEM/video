#ifndef _MIXER_STORAGE_H_
#define _MIXER_STORAGE_H_

#include <stdint.h>

#include "config.h"

#define EEPROM_MATRIX_OFFSET 0x0
#define EEPROM_MUTES_OFFSET 0x100
#define EEPROM_BUS_MULTIPLIER_OFFSET 0x200
#define EEPROM_CHANNEL_MULTIPLIER_OFFSET 0x200

void eeprom_update_byte(uint8_t *addr_ptr, uint8_t data);
void eeprom_update_block(const void *buf, void *addr, uint32_t len);

void eeprom_save_gains(float gains[BUSES][CHANNELS]);
void eeprom_load_gains(float gains[BUSES][CHANNELS]);

void eeprom_save_mutes(uint64_t *mutes);
void eeprom_load_mutes(uint64_t *mutes);

void eeprom_save_bus_multipliers(float multipliers[BUSES]);
void eeprom_load_bus_multipliers(float multipliers[BUSES]);

void eeprom_save_channel_multipliers(float multipliers[CHANNELS]);
void eeprom_load_channel_multipliers(float multipliers[CHANNELS]);

void eeprom_save_all(float gains[BUSES][CHANNELS], uint64_t *mutes, float bus_multipliers[BUSES], float channel_multipliers[CHANNELS]);
void eeprom_load_all(float gains[BUSES][CHANNELS], uint64_t *mutes, float bus_multipliers[BUSES], float channel_multipliers[CHANNELS]);

#endif
