cmake_minimum_required(VERSION 3.17)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc CACHE STRING "C Compiler" FORCE)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++ CACHE STRING "C++ Compiler" FORCE)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc CACHE STRING "ASM Compiler" FORCE)
set(CMAKE_AR arm-none-eabi-ar CACHE STRING "Linker" FORCE)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy CACHE STRING "OBJCOPY" FORCE)
set(CMAKE_OBJDUMP arm-none-eabi-objdump CACHE STRING "OBJDUMP" FORCE)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(teensy_audio C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# Teensy specific
set(VENDOR_ROOT ${CMAKE_SOURCE_DIR}/vendor)
set(COREPATH ${CMAKE_SOURCE_DIR}/vendor/cores/teensy4/)
include_directories(SYSTEM ${COREPATH})
file(GLOB TEENSY_C_FILES ${COREPATH}*.c)
file(GLOB TEENSY_CPP_FILES ${COREPATH}*.cpp)
set(TEENSY_SOURCES ${TEENSY_C_FILES} ${TEENSY_CPP_FILES})
list(REMOVE_ITEM TEENSY_SOURCES ${COREPATH}main.cpp)

# Teensy 4.1 specific configs
set(CPU_DEFINE __IMXRT1062__)
set(CPU_CORE_SPEED 600000000)
set(LINKER_FILE ${COREPATH}imxrt1062_t41.ld)
set(TEENSY_DEFINITIONS USB_MIDI_AUDIO_SERIAL
        LAYOUT_US_ENGLISH
        F_CPU=${CPU_CORE_SPEED}
        ${CPU_DEFINE}
        ARDUINO_TEENSY40)
set(MCPU cortex-m7)
add_compile_options(-mcpu=${MCPU} -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv5-d16 -D${CPU_DEFINE} -DTEENSYDUINO=159 -DARDUINO=10607 -nostdlib)
set(c_flags -MMD -fno-exceptions)
set(cpp_flags -MMD -felide-constructors -fno-exceptions -fno-rtti)
add_compile_options(
        "$<$<COMPILE_LANGUAGE:C>:${c_flags}>"
        "$<$<COMPILE_LANGUAGE:CXX>:${cpp_flags}>"
)
add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16
        )



macro(import_arduino_library LIB_ROOT LIB_NAME)
    # Check if we can find the library.
    if (NOT EXISTS ${LIB_ROOT})
        message(FATAL_ERROR "Could not find the Arduino library directory")
    endif (NOT EXISTS ${LIB_ROOT})
    set(LIB_DIR "${LIB_ROOT}/${LIB_NAME}")
    #message(LIB_DIR ${LIB_DIR})
    if (NOT EXISTS "${LIB_DIR}")
        message(FATAL_ERROR "Could not find the directory for library '${LIB_NAME}'")
    endif (NOT EXISTS "${LIB_DIR}")

    # Add it to the include path.
    include_directories(SYSTEM "${LIB_DIR}")

    # Mark source files to be built along with the sketch code.
    file(GLOB SOURCES_CPP ABSOLUTE "${LIB_DIR}" "${LIB_DIR}/*.cpp")
    foreach (SOURCE_CPP ${SOURCES_CPP})
        set(TEENSY_LIB_SOURCES ${TEENSY_LIB_SOURCES} ${SOURCE_CPP})
    endforeach (SOURCE_CPP ${SOURCES_CPP})
    file(GLOB SOURCES_C ABSOLUTE "${LIB_DIR}" "${LIB_DIR}/*.c")
    foreach (SOURCE_C ${SOURCES_C})
        set(TEENSY_LIB_SOURCES ${TEENSY_LIB_SOURCES} ${SOURCE_C})
    endforeach (SOURCE_C ${SOURCES_C})
    file(GLOB SOURCES_H ABSOLUTE "${LIB_DIR}" "${LIB_DIR}/*.h")
    foreach (SOURCE_H ${SOURCES_H})
        set(TEENSY_LIB_SOURCES ${TEENSY_LIB_SOURCES} ${SOURCE_H})
    endforeach (SOURCE_H ${SOURCES_H})
    file(GLOB SOURCES_S ABSOLUTE "${LIB_DIR}" "${LIB_DIR}/*.S")
    foreach (SOURCE_S ${SOURCES_S})
        set(TEENSY_LIB_SOURCES ${TEENSY_LIB_SOURCES} ${SOURCE_S})
    endforeach (SOURCE_S ${SOURCES_S})

endmacro(import_arduino_library)

import_arduino_library(${VENDOR_ROOT} Wire)
import_arduino_library(${VENDOR_ROOT} SPI)
import_arduino_library(${VENDOR_ROOT} Audio)
import_arduino_library(${VENDOR_ROOT} Audio/utility)
set_property(SOURCE ${VENDOR_ROOT}/Audio/memcpy_audio.S PROPERTY LANGUAGE C)

#import_arduino_library(${VENDOR_ROOT} Adafruit-GFX-Library)
#import_arduino_library(${VENDOR_ROOT} Adafruit_BusIO)
#import_arduino_library(${VENDOR_ROOT} Adafruit_SSD1306)
import_arduino_library(${VENDOR_ROOT} ST7735_t3/src)
import_arduino_library(${VENDOR_ROOT} OSC)

# Build this project
add_executable(${PROJECT_NAME}.elf main.cpp teensy4_usb_descriptor.c effect_dynamics.cpp effect_dynamics.h ${TEENSY_SOURCES} ${TEENSY_LIB_SOURCES})
set_target_properties(${PROJECT_NAME}.elf PROPERTIES COMPILE_FLAGS "-Wall -O2")
set_target_properties(${PROJECT_NAME}.elf PROPERTIES COMPILE_DEFINITIONS "${TEENSY_DEFINITIONS} ${INCLUDE_DIRECTORIES} -D__arm__ ")
set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINK_FLAGS "-Os -Wl,--gc-sections,--defsym=__rtc_localtime=0 --specs=nano.specs -mcpu=${MCPU} -mthumb -T${LINKER_FILE}")
target_include_directories(${PROJECT_NAME}.elf PRIVATE ${INCLUDE_DIRECTORIES})

# Build HEX file
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")