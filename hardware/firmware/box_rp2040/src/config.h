#pragma once
#include <hardware/i2c.h>
#include <pico/stdlib.h>

// For wiring GPIO pins to the Radxa X2L, you can use the following pinout
// of the Radxa GPIO connector:
//
//  +3.3V -o 1    40 o- +5V
//   GP28 -o 2    39 o- +5V
//   GP29 -o 3    38 o- GND
//   GP04 -o 4    37 o- GP20
//    GND -o 5    36 o- GP21
//   GP05 -o 6    35 o- GP23
//   GP06 -o 7    34 o- GND
//   GP03 -o 8    33 o- GP22
//  +3.3V -o 9    32 o- GP27
//   GP11 -o 10   31 o- GND
//   GP08 -o 11   30 o- GP24
//   GP10 -o 12   29 o- GP09
//    GND -o 13   28 o- GP18
//   GP16 -o 14   27 o- GP17
//   GP07 -o 15   26 o- GND
//   GP12 -о 16   25 о- GP19
//   GP13 -o 17   24 o- GND
//   GP15 -o 18   23 o- GP26
//   GP14 -o 19   22 o- GP02
//    GND -o 20   21 o- GP25
//
// Note: the pin header silkscreen text is flipped on the Radxa
// board (the silkscreen next to pin1 says "40").
// The pins in this diagram follow the numbering
// from the radxa schematic and not the silkscreen.
// The actual Raspberry Pi GPIO header follows an odd/even
// numbering scheme that is different from the radxa connector scheme.
// +3.3V pins are coloured yellow, +5V pins are coloured red
// and GND pins are coloured black (colours are correct).
//
// All pin definitions in this file correspond to GPIO pin
// numbers on the RP2040: thus, if PIN_FOO is defined to 42,
// it means that it is wired to GP42.

// ********** USB ***********
// set to -1 to disable USB
#define USB_INSTANCE                0
#define USB_CDC_WRITE_TIMEOUT_US    500000
#define USB_MAGIC_BAUD_RESTART_BL   134
#define USB_READ_BUF_SIZE           1024

// ********* UART ***********
// set to uart0, etc to enable or to NULL to disable UART
#define UART_INST                   NULL
#define UART_BAUD                   115200
#define UART_PIN_TX                 0
#define UART_PIN_RX                 1

// ******** DISPLAY *********
#define DISPLAY_SPI                 spi1

// If using the Waveshare HAT display, it has the following pinout:
//
//    +5V -o 2     1 o- +3.3V
//    +5V -o 4     3 o-
//    GND -o 6     5 o-
//        -o 8     7 o- D1_DC
//        -o 10    9 o- GND
//  D0_CS -o 12   11 o-
//    GND -o 14   13 o- D0_RST
// D2_RST -o 16   15 o- D0_DC
// D1_RST -o 18   17 o- +3.3V
//    GND -o 20   19 o- D12_DAT
//   BTN0 -o 22   21 o-
//  D1_CS -o 24   23 o- D12_CLK
//  D2_CS -o 26   25 o- GND
//        -o 28   27 o-
//    GND -o 30   29 o- D2_DC
//  D2_BL -о 32   31 о-
//    GND -o 34   33 o- D1_BL
//        -o 36   35 o- D0_BL
// D0_DAT -o 38   37 o- BTN1
// D0_CLK -o 40   39 o- GND
//
// The pins that need to be wired are D0_*, one GND, one +3.3V and one +5V
// When using another type of display, the pins will probably be named DAT, CLK, etc

#define DISPLAY_PIN_DAT                 11  // radxa pin 10 (next to 3v3)
#define DISPLAY_PIN_CLK                 10  // radxa pin 12 (next to gnd)
#define DISPLAY_PIN_CS                  13  // radxa pin 17
#define DISPLAY_PIN_DC                  19  // radxa pin 25 (between two gnd's)
#define DISPLAY_PIN_RST                 20  // radxa pin 37 (next to gnd)
#define DISPLAY_PIN_BL                  22  // radxa pin 33 (next to gnd), not present on all displays

#define DISPLAY_WIDTH                   240
#define DISPLAY_HEIGHT                  240
#define DISPLAY_ROTATION                ST7789_PORTRAIT
#define DISPLAY_TEXT_COLS               42
#define DISPLAY_TEXT_LINES              10

#define COLOUR_SCHEME_GRUVBOX

// ****** NETWORK SWITCH INTERFACE *******
// on the switch itself, the pins are in the following order:
//      GND, DAT, CLK, NC (we don't use the last one)
// you can recognise the GND by the fact that it is solid-connected
// to the GND filled plane
#define NSW_PIN_DAT                     14  // radxa pin 19 (next to gnd)
#define NSW_PIN_CLK                     15  // radxa pin 18

#define NSW_NUM_PORTS                   5

#define PWR_BRD_I2C_INST                i2c1
#define PWR_BRD_I2C_SDA                 26
#define PWR_BRD_I2C_SCL                 27
#define PWR_BRD_I2C_INT                 17
#define PWR_BRD_I2C_INT_AUX             18
#define PWR_BRD_I2C_BAUD                (10 * 1000)
#define PWR_BRD_I2C_TIMEOUT_US          20000
#define PWR_BRD_FAN_CTL_ADDR            0x4d
#define PWR_BRD_TEMP_SENS_ADDR          0x4b
#define PWR_BRD_EXPANDER_ADDR           0b0100010
#define PWR_BRD_EXPANDER_PIN_DIRS       0b11111011
#define PWR_BRD_EXPANDER_PIN_CHG_PWR    2
