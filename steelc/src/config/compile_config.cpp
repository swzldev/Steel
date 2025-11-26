#include "compile_config.h"

#include <string>

#include "console_args.h"
#include "bad_arg_exception.h"
#include "../utils/maybe.h"

compile_config compile_config::from_console_args(const console_args& args) {
	compile_config cfg;
	if (args.has_arg("--print-src")) {
		cfg.print_src = true;
	}
	if (args.has_arg("--print-tokens")) {
		cfg.print_tokens = true;
	}
	if (args.has_arg("--print-tokenized-input")) {
		cfg.print_tokenized_input = true;
	}
	if (args.has_arg("--print-ast")) {
		cfg.print_ast = true;
	}
	return cfg;
}