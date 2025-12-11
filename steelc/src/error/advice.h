#pragma once

#include <string>

#include <lexer/token.h>

struct error;

struct advice_info {
	std::string code;
	std::string message;
};

struct advice {
	error* parent;
	advice_info info;
};