#pragma once

#include <string>

#include <lexer/token_type.h>

struct position {
	position() = default;
	position(size_t line, size_t column)
		: line(line), column(column) {
	}

	size_t line = 0;
	size_t column = 0;
};

struct code_span {
	code_span() = default;
	code_span(position start, position end)
		: start(start), end(end) {
	}

	position start;
	position end;
};

struct token {
	std::string value;
	token_type type;
	code_span span;
};