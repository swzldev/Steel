#pragma once

#include <string>
#include <memory>

#include "../lexer/token.h"

class compilation_unit;

enum error_type {
	ERR_ERROR,
	ERR_WARNING,
	ERR_INFO,
};

struct error_info {
	std::string code;
	std::string message;
};

struct error {
	error_info info;
	position pos;
	error_type type;
	std::shared_ptr<compilation_unit> unit;
};