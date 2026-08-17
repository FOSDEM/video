#include "phantomiser.h"

#include <math.h>

float signf(float x) {
	if (signbit(x)) {
		return -1;
	}
	return 1;
}

static float linear_ramp_to(float x, float step, float target) {
	step      = fabs(step);
	float err = x - target;
	if (fabs(err) < step) {
		return target;
	}
	return x - step * signf(err);
}

void Phantomiser::begin() {
	for (uint8_t i = 0; i < PHANTOM_NUM; i++) {
		this->hardwarePwm[i] = isPwmPin(pins[i]);
	}
}

void Phantomiser::setPhantom(uint8_t id, bool val) {
	if (id >= PHANTOM_NUM) {
		return;
	}
	this->wantedState[id] = val;
}

void Phantomiser::update() {
	unsigned long t = micros();
	this->dt        = float(t - this->t) / 1e6;
	this->t         = t;
	if (dt <= 0) {
		return;
	}

	if (this->idBeingActivated != 0xff) {
		if (this->updateActivation(this->idBeingActivated)) {
			this->idBeingActivated = 0xff;
		}
		return;
	}

	for (uint8_t i = 0; i < PHANTOM_NUM; i++) {
		if (this->duties[i] != float(this->wantedState[i])) {
			this->idBeingActivated = i;
			return;
		}
	}
}

bool Phantomiser::updateActivation(uint8_t i) {
	float speed;
	if (this->duties[i] <= float(this->wantedState[i])) {
		speed = PHANTOM_ACTIVATION_SPEED_ON;
	} else {
		speed = PHANTOM_ACTIVATION_SPEED_OFF;
	}

	this->duties[i] = linear_ramp_to(
		this->duties[i],
		speed * this->dt,
		float(this->wantedState[i])
	);

	this->applyDuty(i, this->duties[i]);

	return this->duties[i] == float(this->wantedState[i]);
}

void Phantomiser::applyDuty(uint8_t i, float duty) {
	if (duty <= 0.0f) {
		pinMode(this->pins[i], OUTPUT);
		digitalWrite(this->pins[i], LOW);
		return;
	}
	if (duty >= 1.0f) {
		pinMode(this->pins[i], OUTPUT);
		digitalWrite(this->pins[i], HIGH);
		return;
	}

	float curvedDuty = powf(duty, PHANTOM_ACTIVATION_CURVE);

	if (this->hardwarePwm[i]) {
		analogWrite(this->pins[i], uint8_t(curvedDuty * 255.0f + 0.5f));
	} else {
		digitalWrite(this->pins[i], HIGH);
		delayMicroseconds(uint32_t(curvedDuty * 255.0f + 1.0f));
		digitalWrite(this->pins[i], HIGH);
	}
}
