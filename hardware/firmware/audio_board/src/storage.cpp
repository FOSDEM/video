#include <avr/eeprom.h>

#include "config.h"
#include "storage.h"

#include "debug.h"
#include "LittleFS.h"

LittleFS_Program configfs;
File handle;

void storage_print()
{
    File dir = configfs.open("/");
    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            debug_printf("-- no more files --\n");
            break;
        }
        if (entry.isDirectory())
        {
            debug_printf("- %s/\n", entry.name());
        }
        else
        {
            debug_printf("- %s [%d]\n", entry.name(), entry.size());
        }
    }
}

void storage_setup()
{
    if (!configfs.begin(1024 * 1024))
    {
        debug_printf("Error opening LittleFS\n");
    }
    storage_print();
}

bool storage_save(const char* name, const void* data, uint32_t len)
{
    handle = configfs.open(name, FILE_WRITE_BEGIN);
    if (!handle)
    {
        debug_printf("Could not open file %s for writing\n", name);
        return false;
    }
    handle.truncate();
    handle.write(data, len);
    handle.close();
    return true;
}

bool storage_load(const char* name, void* data, uint32_t len)
{
    handle = configfs.open(name, FILE_READ);
    if (!handle)
    {
        debug_printf("Could not open file %s\n", name);
        return false;
    }
    if (len != handle.size())
    {
        debug_printf("File size mismatch file=%d, struct=%d\n", (uint16_t)handle.size(), len);
        return false;
    }
    handle.read(data, len);
    handle.close();
    return true;
}

uint8_t eeprom_update_byte(uint8_t* addr_ptr, uint8_t data)
{
    if (data != eeprom_read_byte(addr_ptr))
    {
        eeprom_write_byte(addr_ptr, data);
        return 1;
    }
    return 0;
}

// Reimplements the avr lib function, as the Teensyduino lib does not provide
// a conditional write function
uint8_t eeprom_update_block(const void* buf, void* addr, uint32_t len)
{
    uint8_t written = 0;
    uint8_t* p = (uint8_t*)addr;
    const uint8_t* src = (const uint8_t*)buf;
    while (len--)
    {
        written += eeprom_update_byte(p++, *src++);
    }

    return written;
}

uint8_t eeprom_save_all(AudioState& state, uint8_t* offset)
{
    return eeprom_update_block(&state, (uint8_t*)offset, sizeof(state));
}

void eeprom_load_all(AudioState& state, uint8_t* offset)
{
    eeprom_read_block(&state, (uint8_t*)offset, sizeof(state));
}
