#include <avr/eeprom.h>

#include "config.h"
#include "storage.h"

uint8_t eeprom_update_byte(uint8_t *addr_ptr, uint8_t data) {
    if (data != eeprom_read_byte(addr_ptr)) {
        eeprom_write_byte(addr_ptr, data);
        return 1;
    }
    return 0;
}

// Reimplements the avr lib function, as the Teensyduino lib does not provide
// a conditional write function
uint8_t eeprom_update_block(const void *buf, void *addr, uint32_t len) {
    uint8_t written = 0;
    uint8_t *p = (uint8_t *)addr;
    const uint8_t *src = (const uint8_t *)buf;
    while (len--) {
        written += eeprom_update_byte(p++, *src++);
    }

    return written;
}

uint8_t eeprom_save_all(AudioState &state, uint8_t *offset) {
    return eeprom_update_block(&state, (uint8_t *)offset, sizeof(state));
}
void eeprom_load_all(AudioState &state, uint8_t *offset) {
    eeprom_read_block(&state, (uint8_t *)offset, sizeof(state));
}
