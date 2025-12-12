#pragma once

#include <string>

#include <input/console_args.h>
//#include <utils/maybe.h>

class compile_config {
public:
	compile_config() = default;

	static compile_config from_console_args(const console_args& args);
};