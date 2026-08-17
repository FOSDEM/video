#pragma once

#include <Arduino.h>

#include "config.h"

class Phantomiser {
	public:
		void begin();
		void setPhantom(uint8_t id, bool val);
		void update();

	private:
		bool updateActivation(uint8_t i);
		void applyDuty(uint8_t i, float duty);

		const uint8_t pins[PHANTOM_NUM] = PHANTOM_PINS;
		float         duties[PHANTOM_NUM]{};
		bool          wantedState[PHANTOM_NUM]{};

		uint8_t idBeingActivated = 0xff;

		bool    isHardwarePWM[PHANTOM_NUM]{};
		uint8_t softPWMcounter;
		void    softPWM(int pin, uint8_t intDuty);

		unsigned long t{};
		float         dt{};
};
