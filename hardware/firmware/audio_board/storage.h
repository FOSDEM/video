#ifndef _MIXER_STORAGE_H_
#define _MIXER_STORAGE_H_

#include <stdint.h>

#include "config.h"

#define EEPROM_MATRIX_OFFSET 0x0
#define EEPROM_MUTES_OFFSET 0x100

void eeprom_update_byte(uint8_t *addr_ptr, uint8_t data);
void eeprom_update_block(const void *buf, void *addr, uint32_t len);

void eeprom_save_gains(float gains[BUSES][CHANNELS]);
void eeprom_load_gains(float gains[BUSES][CHANNELS]);

void eeprom_save_mutes(uint64_t *mutes);
void eeprom_load_mutes(uint64_t *mutes);

#endif
