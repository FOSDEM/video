#include <Arduino.h>

class Cli {
	public:
		Cli(Stream*);
		void update();

	private:
		Stream* port;

		struct CmdDescr {
				const char*               name;
				const char*               help;
				const char*               arghelp;
				int32_t                   num_args;
				std::function<void(Cli*)> callback;
		};

		static const uint8_t  num_cmds = 18;
		static const CmdDescr cmds[num_cmds + 1];

		char    input_buf[100];
		uint8_t input_pos = 0;

		void exec_cmd();
		bool is_terminator(char c);
		bool is_terminator_or_whitespace(char c);
		void skip_whitespace_in(char** buf);
		void skip_whitespace();

		uint16_t             hop_uint();
		float                hop_float();
		bool                 hop_word(const char* word);
		const Cli::CmdDescr* hop_cmd();

		void eat(char chr);

		void print_float_fixed(float x, uint8_t whole_digits, uint8_t frac_digits);
		void print_usage(const Cli::CmdDescr& cmd);

		char* cmd;

		uint16_t slug;

		void print_fail();
		void prefix_fail();
		void print_ok();
		void prefix_ok();
		void report_ok();
};
