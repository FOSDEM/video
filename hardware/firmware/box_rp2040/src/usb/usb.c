#include "usb.h"
#include "config.h"
#include "pico/bootrom.h"
// TODO: implement vendor-reset interface

void usb_init(void) {
    if (!USB_ENABLED) {
        return;
    }
    tud_init(USB_INSTANCE);
}

void usb_task(void) {
    if (!USB_ENABLED) {
        return;
    }
    tud_task();
}

bool usb_cdc_connected(void) {
    return USB_ENABLED && tud_cdc_connected();
}

void usb_cdc_write(const char *buf, int length) {
    // this is an almost-copy-paste from the pi pico stdio implementation
    static uint64_t last_avail_time;
    if (usb_cdc_connected()) {
        for (int i = 0; i < length;) {
            int n = length - i;
            int avail = (int) tud_cdc_write_available();
            if (n > avail) n = avail;
            if (n) {
                int n2 = (int) tud_cdc_write(buf + i, (uint32_t)n);
                tud_task();
                tud_cdc_write_flush();
                i += n2;
                last_avail_time = time_us_64();
            } else {
                tud_task();
                tud_cdc_write_flush();
                if (!usb_cdc_connected() ||
                    (!tud_cdc_write_available() && time_us_64() > last_avail_time + USB_CDC_WRITE_TIMEOUT_US)) {
                    break;
                }
            }
        }
    } else {
        // reset our timeout
        last_avail_time = 0;
    }
}

int32_t usb_cdc_read(char* buf, uint32_t size) {
    if (!usb_cdc_connected()) {
        return -2;
    }

    return tud_cdc_read(buf, size);
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding) {
    uint32_t baud = p_line_coding->bit_rate;
    if (baud == USB_MAGIC_BAUD_RESTART_BL) {
        // when baud is set to the magic number, restart into bootloader
        reset_usb_boot(0, 0);
    }
}

bool vendor_request_temperature_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
    if (request->bmRequestType_bit.direction != TUSB_DIR_IN)
	    return false;

    if (stage != CONTROL_STAGE_SETUP)
	    return true;

    uint8_t reg = 0;
    uint8_t buffer[2] = {0};
    i2c_write_blocking_until(PWR_BRD_I2C_INST, PWR_BRD_TEMP_SENS_ADDR, &reg, 1, true, make_timeout_time_ms(20));
    i2c_read_blocking_until(PWR_BRD_I2C_INST, PWR_BRD_TEMP_SENS_ADDR, &buffer, 2, false, make_timeout_time_ms(20));
    io_say_f("  TEMP: [%d, %d]\r\n", buffer[0], buffer[1]);

    return tud_control_xfer(rhport, request, &buffer, sizeof(buffer));
}

bool vendor_request_fanspeed_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
    if (request->bmRequestType_bit.direction != TUSB_DIR_IN)
	    return false;

    if (stage != CONTROL_STAGE_SETUP)
	    return true;

    uint8_t buffer[2] = {0};
    uint16_t fs = 0;
    fan_ctl_get_fan_speed(request->wIndex, &fs);
    buffer[0] = (fs>>8) & 0xFF;
    buffer[1] = (fs>>0) & 0xFF;
    io_say_f("  FANSPEED %d: %d\r\n", request->wIndex, fs);
    return tud_control_xfer(rhport, request, &buffer, sizeof(buffer));
}


bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const* request) {
    bool result = false;
    switch (request->bmRequestType_bit.type) {
        case TUSB_REQ_TYPE_VENDOR:
            io_say_f("XFER: bRequest=%d, wValue=%d, wIndex=%d, wLength=%d\r\n", request->bRequest, request->wValue, request->wIndex, request->wLength);
            switch (request->bRequest) {
                case VENDOR_REQUEST_TEMPERATURE:
                    result = vendor_request_temperature_cb(rhport, stage, request);
                    break;
                case VENDOR_REQUEST_FANSPEED:
                    result = vendor_request_fanspeed_cb(rhport, stage, request);
                    break;
            }
            break;
        default:
            break;
    }
    return result;
}
