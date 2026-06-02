#include "debug.h"

#include <cstdint>
#include <cstdio>

#include <Arduino.h>

void debug_init() {
	if (CrashReport) {
		while (!SerialUSB && millis() < 1500)
			;
		SerialUSB.print(CrashReport);
	}
}

void debug_printf(const char* format, ...) {
	char    buf[256];
	va_list list;
	va_start(list, format);
	vsnprintf(buf, sizeof(buf), format, list);
	va_end(list);
	debug_print(buf);
}

void debug_println(const char* buf) {
	debug_print(buf);
	debug_print("\n");
}

static uint16_t cur_line_idx = 0;
static char     cur_line[256];

void debug_print_line(const char* buf) {
	SerialUSB.print("[log] ");
	SerialUSB.println(buf);
}

void debug_print(const char* buf) {
	for (uint8_t i = 0; buf[i] != '\0'; i++) {
		char c = buf[i];
		if (c == '\r' || c == '\n') {
			cur_line[cur_line_idx] = '\0';
			debug_print_line(cur_line);
			cur_line_idx = 0;
		} else {
			cur_line[cur_line_idx] = c;
			cur_line_idx++;
		}
	}
}
