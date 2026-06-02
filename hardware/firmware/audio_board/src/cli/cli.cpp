#include "cli.h"

Cli::Cli(Stream* _port) : port(_port) {}

void Cli::exec_cmd() {
	uint8_t num_args = 0;
	for (uint8_t i = 0; this->input_buf[i] != '\0'; i++) {
		if (this->input_buf[i] == ' ') {
			num_args++;
		}
	}

	if (this->cmd[0] >= '0' && this->cmd[0] <= '9') {
		if (num_args == 0) {
			this->prefix_fail();
			this->port->println("I don't speak in numbers (say `help`)");
			return;
		}
		this->slug = this->hop_uint();
		num_args--;
	}

	if (this->cmds[this->num_cmds].name == nullptr || strncmp("END", this->cmds[this->num_cmds].name, 3) != 0) {
		this->prefix_fail();
		this->port->println("internal bug: num_cmds in cli.h is probably wrong");
		return;
	}

	const Cli::CmdDescr* cmd = this->hop_cmd();
	if (!cmd) {
		this->prefix_fail();
		this->port->println("unknown command; use `help` for help");
		return;
	}

	if (cmd->num_args >= 0 && cmd->num_args != num_args) {
		this->prefix_fail();
		this->print_usage(*cmd);
		this->port->println(")");
		return;
	}

	cmd->callback(this);
}

void Cli::print_usage(const Cli::CmdDescr& cmd) {
	this->port->print("usage: ");
	this->port->print(cmd.name);
	if (cmd.arghelp[0] != '\0') {
		this->port->print(" ");
		this->port->print(cmd.arghelp);
	}
	this->port->print(" -- ");
	this->port->print(cmd.help);
}

const Cli::CmdDescr* Cli::hop_cmd() {
	for (uint8_t i = 0; i < Cli::num_cmds; i++) {
		const Cli::CmdDescr& cmd = this->cmds[i];
		if (cmd.name == nullptr || cmd.help == nullptr || cmd.arghelp == nullptr) {
			this->prefix_fail();
			this->port->printf("internal bug: entry %d in commands.cpp is corrupted", i);
			return nullptr;
		}

		if (this->hop_word(cmd.name)) {
			return &cmd;
		}
	}
	return nullptr;
}

void Cli::print_float_fixed(float x, uint8_t whole_digits, uint8_t frac_digits) {
	char buf[whole_digits + frac_digits + 3];

	bool sign = x < 0;
	if (sign) {
		x = -x;
	}

	buf[0] = sign ? '-' : '+';

	float limit = 1;
	for (uint8_t i = 0; i < whole_digits; i++) {
		limit *= 10;
	}

	if (x >= limit) {
		buf[1] = 'i';
		buf[2] = 'n';
		buf[3] = 'f';
		buf[4] = '\0';
	} else {
		uint32_t whole = (uint32_t)x;
		float    frac  = x - whole;

		for (uint8_t i = whole_digits; i >= 1; i--) {
			buf[i] = '0' + (whole % 10);
			whole /= 10;
		}

		buf[whole_digits + 1] = '.';

		for (uint8_t i = 0; i < frac_digits; i++) {
			frac *= 10;
			buf[whole_digits + 2 + i] = '0' + (uint32_t)frac;
			frac -= (uint32_t)frac;
		}

		buf[frac_digits + whole_digits + 2] = '\0';
	}

	// FIXME: give this function to a year 1 uni student to make prettier
	if (sign) {
		this->port->print(buf);
	} else {
		this->port->print(buf + 1);
	}
}

bool Cli::is_terminator(char c) {
	return c == '\n' || c == '\r' || c == '\0';
}

bool Cli::is_terminator_or_whitespace(char c) {
	return is_terminator(c) || c == ' ' || c == '\t';
}

void Cli::skip_whitespace_in(char** buf) {
	while (**buf == ' ') {
		(*buf)++;
	}
}

void Cli::skip_whitespace() {
	skip_whitespace_in(&this->cmd);
}

uint16_t Cli::hop_uint() {
	skip_whitespace();
	uint16_t result = 0;
	while (*this->cmd >= '0' && *this->cmd <= '9') {
		result *= 10;
		result += (*this->cmd) - '0';
		this->cmd++;
	}

	if (*this->cmd == ' ') {
		this->cmd++;
	}
	return result;
}

float Cli::hop_float() {
	skip_whitespace();

	float sign = 1.0f;
	if (*this->cmd == '-') {
		sign = -1.0f;
		this->cmd++;
	} else if (*this->cmd == '+') {
		this->cmd++;
	}

	float result = 0.0f;
	while (*this->cmd >= '0' && *this->cmd <= '9') {
		result *= 10.0f;
		result += (*this->cmd) - '0';
		this->cmd++;
	}

	if (*this->cmd == '.') {
		this->cmd++;
		float place = 0.1f;
		while (*this->cmd >= '0' && *this->cmd <= '9') {
			result += ((*this->cmd) - '0') * place;
			place *= 0.1f;
			this->cmd++;
		}
	}

	if (*this->cmd == ' ') {
		this->cmd++;
	}

	return sign * result;
}

bool Cli::hop_word(const char* word) {
	char* buf = this->cmd;
	skip_whitespace_in(&buf);

	while (*word != 0 && !is_terminator_or_whitespace(*buf) && *buf == *word) {
		buf++;
		word++;
	}

	if (*word != '\0' || !is_terminator_or_whitespace(*buf)) {
		return false;
	}

	skip_whitespace_in(&buf);

	this->cmd = buf;
	return true;
}

void Cli::eat(char chr) {
	if (this->input_pos > sizeof(this->input_buf) - 2) {
		if (is_terminator(chr)) {
			this->input_pos    = 0;
			this->input_buf[0] = '\0';
			this->port->write("[err] line too long\n");
		}
		return;
	}
	this->input_buf[this->input_pos] = chr;
	if (!is_terminator(chr)) {
		this->input_pos++;
		return;
	}
	this->input_buf[this->input_pos] = '\0';
	this->input_pos                  = 0;
	if (this->input_buf[0] == '\0') {
		return; // empty line
	}

	this->cmd = &this->input_buf[0];

	exec_cmd();
}

void Cli::update() {
	while (this->port->available()) {
		int c = this->port->read();
		if (c > 0) {
			this->eat((char)c);
		}
	}
}

void Cli::print_fail() {
	if (this->slug > 0) {
		this->port->printf("[%d err]", this->slug);
		this->slug = 0;
	} else {
		this->port->printf("[err]");
	}
}

void Cli::prefix_fail() {
	this->print_fail();
	this->port->print(" ");
}

void Cli::print_ok() {
	if (this->slug > 0) {
		this->port->printf("[%d ok]", this->slug);
		this->slug = 0;
	} else {
		this->port->printf(" [ok]");
	}
}

void Cli::prefix_ok() {
	this->print_ok();
	this->port->print(" ");
}

void Cli::report_ok() {
	this->print_ok();
	this->port->println();
}
