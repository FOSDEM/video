#include <avr/eeprom.h>

#include "storage.h"
#include "config.h"

void
eeprom_update_byte(uint8_t *addr_ptr, uint8_t data) {
        if(data != eeprom_read_byte(addr_ptr))
                eeprom_write_byte(addr_ptr, data);
}

// Reimplements the avr lib function, as the Teensyduino lib does not provide
// a conditional write function
void
eeprom_update_block(const void *buf, void *addr, uint32_t len)
{
        uint8_t *p = (uint8_t *)addr;
        const uint8_t *src = (const uint8_t *)buf;
        while (len--) {
                eeprom_update_byte(p++, *src++);
        }
}

void
eeprom_save_gains(float gains[BUSES][CHANNELS])
{
        eeprom_update_block(gains, (size_t*) EEPROM_MATRIX_OFFSET, sizeof(float) * BUSES * CHANNELS);
}

void
eeprom_load_gains(float gains[BUSES][CHANNELS])
{
        eeprom_read_block(gains, (size_t*) EEPROM_MATRIX_OFFSET, sizeof(float) * BUSES * CHANNELS);
}

void
eeprom_save_mutes(uint64_t *mutes)
{
        eeprom_update_block(mutes, (size_t*) EEPROM_MUTES_OFFSET, sizeof(mutes));
}

void
eeprom_load_mutes(uint64_t *mutes)
{
        eeprom_read_block(mutes, (size_t*) EEPROM_MUTES_OFFSET, sizeof(mutes));
}
