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
	eeprom_update_block(gains, (uint8_t*) EEPROM_MATRIX_OFFSET, sizeof(float) * BUSES * CHANNELS);
}

	void
eeprom_load_gains(float gains[BUSES][CHANNELS])
{
	eeprom_read_block(gains, (uint8_t*) EEPROM_MATRIX_OFFSET, sizeof(float) * BUSES * CHANNELS);
}

	void
eeprom_save_mutes(uint64_t *mutes)
{
	eeprom_update_block(mutes, (uint8_t*) EEPROM_MUTES_OFFSET, sizeof(mutes));
}

	void
eeprom_load_mutes(uint64_t *mutes)
{
	eeprom_read_block(mutes, (uint8_t*) EEPROM_MUTES_OFFSET, sizeof(mutes));
}

void eeprom_save_channel_multipliers(float multipliers[CHANNELS]) {
	eeprom_update_block(multipliers, (uint8_t*) EEPROM_CHANNEL_MULTIPLIER_OFFSET, sizeof(float) * CHANNELS);
}
void eeprom_load_channel_multipliers(float multipliers[CHANNELS]) {
	eeprom_read_block(multipliers, (uint8_t*) EEPROM_CHANNEL_MULTIPLIER_OFFSET, sizeof(float) * CHANNELS);
}

void eeprom_save_bus_multipliers(float multipliers[BUSES]) {
	eeprom_update_block(multipliers, (uint8_t*) EEPROM_BUS_MULTIPLIER_OFFSET, sizeof(float) * BUSES);
}
void eeprom_load_bus_multipliers(float multipliers[BUSES]) {
	eeprom_read_block(multipliers, (uint8_t*) EEPROM_BUS_MULTIPLIER_OFFSET, sizeof(float) * BUSES);
}


void eeprom_save_all(float gains[BUSES][CHANNELS], uint64_t *mutes, float bus_multipliers[BUSES], float channel_multipliers[CHANNELS]){
	eeprom_save_gains(gains);
        eeprom_save_mutes(mutes);
        eeprom_save_bus_multipliers(bus_multipliers);
        eeprom_save_channel_multipliers(channel_multipliers);
}
void eeprom_load_all(float gains[BUSES][CHANNELS], uint64_t *mutes, float bus_multipliers[BUSES], float channel_multipliers[CHANNELS]){
	eeprom_load_gains(gains);
        eeprom_load_mutes(mutes);
        eeprom_load_bus_multipliers(bus_multipliers);
        eeprom_load_channel_multipliers(channel_multipliers);
}

