#include "config.h"
#include <stdint.h>

#include "teensyaudio.h"
#include "cli/cli.h"
#include "debug.h"

#ifdef USE_EEPROM

#include "storage.h"

#endif

#ifdef USE_DISPLAY

#include "display.h"

#endif

void setup() {
	SerialUSB.begin(115200);
	debug_init();
	debug_print("board ready");

#ifdef USE_DISPLAY
	display_setup();
#endif
	audio_load_state();

	audio_setup();
}

Cli the_cli(&SerialUSB);

unsigned long last_draw = 0;
unsigned long last_save = 0;

void loop() {
	int size;

	Levels& levels = audio_get_levels();

	audio_update_levels(levels);

#ifdef USE_DISPLAY
	display_update_vu(levels.rms);

	if (last_draw < (millis() - 16)) {
		display_update_screen();
		last_draw = millis();
	}
#endif

	the_cli.update();

#ifdef USE_EEPROM
	// save to EEPROM every 60 seconds
	if (last_save + 60000 < millis()) {
		size      = audio_eeprom_save_all();
		last_save = millis();

		debug_printf("eeprom: wrote %d bytes\n", size);
	}
#endif
}

int main() {
	setup();
	while (1) {
		loop();
	}
};
