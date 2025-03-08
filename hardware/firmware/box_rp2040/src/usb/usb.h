#pragma once

#include "tusb.h"
#include "config.h"

#include <stdbool.h>

#define USB_ENABLED (USB_INSTANCE >= 0)

#define VENDOR_REQUEST_TEMPERATURE (0x10)
#define VENDOR_REQUEST_FANSPEED (0x11)

void usb_init(void);
void usb_task(void);

bool usb_cdc_connected(void);
void usb_cdc_write(const char *buf, int length);
int32_t usb_cdc_read(char* buf, uint32_t size);
