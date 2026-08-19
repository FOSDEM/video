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
		pinMode(this->pins[i], OUTPUT);
		digitalWriteFast(this->pins[i], LOW);
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
	int      pin        = this->pins[i];
	float    curvedDuty = powf(duty, PHANTOM_ACTIVATION_CURVE);
	uint32_t pwmPeriod  = curvedDuty * PHANTOM_SOFTPWM_PERIOD_US;

	if (pwmPeriod == 0) {
		this->softPWMsettle(pin, LOW);
		return;
	}
	if (pwmPeriod >= PHANTOM_SOFTPWM_PERIOD_US) {
		this->softPWMsettle(pin, HIGH);
		return;
	}

	this->softPWMperiod = pwmPeriod;
	if (this->softPWMstate == Off) {
		noInterrupts();
		digitalWriteFast(pin, LOW);
		this->softPWMstate = Low;
		bool success = this->softPWMtimer.begin(
			[this] { this->softPWMtick(); },
			pwmPeriod
		);
		if (!success) {
			this->softPWMstate = Off;
		}
		interrupts();
	}
}

void Phantomiser::softPWMsettle(int pin, bool level) {
	this->softPWMstate = Off;
	digitalWriteFast(pin, level);
}

void Phantomiser::softPWMtick() {
	uint32_t period = this->softPWMperiod;

	// softPWMtimer.update() has 1 period latency, so each Low period
	// calls update with the period for Low, which will take effect at
	// the period after the next one, instead of calling update with High
	switch (this->softPWMstate) {
	case High:
		digitalWriteFast(this->pins[this->idBeingActivated], LOW);
		this->softPWMstate = Low;
		this->softPWMtimer.update(period);
		return;
	case Low:
		digitalWriteFast(this->pins[this->idBeingActivated], HIGH);
		this->softPWMstate = High;
		this->softPWMtimer.update(PHANTOM_SOFTPWM_PERIOD_US - period);
		return;
	case Off:
		this->softPWMtimer.end();
		return;
	}
}
