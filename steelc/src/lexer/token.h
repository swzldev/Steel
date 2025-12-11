#pragma once

#include <string>

#include <lexer/token_type.h>

struct position {
	size_t line;
	size_t column;
};

struct token {
	std::string value;
	token_type type;
	position pos;
};