#pragma once
#include <cstdint>

#include "config.h"
#include "debug.h"
#include "LittleFS.h"
#include "types.h"

#define EEPROM_EPOCH (1)

void storage_setup();
bool storage_save(const char* name, const void* data, uint32_t len);
bool storage_load(const char* name, void* data, uint32_t len);

uint8_t eeprom_update_byte(uint8_t* addr_ptr, uint8_t data);
uint8_t eeprom_update_block(const void* buf, void* addr, uint32_t len);

uint8_t eeprom_save_all(AudioState& state, uint8_t* offset);
void    eeprom_load_all(AudioState& state, uint8_t* offset);

template <typename T>
const T& eeprom_put(uint8_t idx, const T& t)
{
    auto src = (const size_t*)&t;

    for (size_t count = sizeof(T); count; --count, ++idx)
    {
        eeprom_update_byte(&idx, *src++);
    }
    return t;
}

template <typename T>
T& eeprom_get(uint8_t idx, T& t)
{
    auto dst = (size_t*)&t;
    for (size_t count = sizeof(T); count; --count, ++idx)
    {
        *dst++ = eeprom_read_byte(&idx);
    }
    return t;
}
template <typename T>
uint8_t struct_checksum( T& t)
{
    // Use the EEPROM_EPOCH constant to invalidate all structs quickly
    uint8_t checksum = EEPROM_EPOCH;
    auto src = reinterpret_cast<uint8_t*>(&t);
    for (size_t count = sizeof(T); count; --count)
    {
        checksum -= *src++;
    }
    return checksum;
}