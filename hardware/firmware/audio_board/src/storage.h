#ifndef _MIXER_STORAGE_H_
#define _MIXER_STORAGE_H_

#include <stdint.h>

#include "config.h"
#include "types.h"

uint8_t eeprom_update_byte(uint8_t *addr_ptr, uint8_t data);
uint8_t eeprom_update_block(const void *buf, void *addr, uint32_t len);

uint8_t eeprom_save_all(AudioState &state, uint8_t *offset);
void eeprom_load_all(AudioState &state, uint8_t *offset);
#endif
