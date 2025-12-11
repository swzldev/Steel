#pragma once

#include <string>

#include <input/console_args.h>
//#include <utils/maybe.h>

class compile_config {
public:
	compile_config() = default;

	static compile_config from_console_args(const console_args& args);

	bool print_src = false;
	bool print_tokens = false;
	bool print_tokenized_input = false;
	bool print_ast = false;
};