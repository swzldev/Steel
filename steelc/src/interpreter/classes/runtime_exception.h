#pragma once

#include <stdexcept>

class runtime_exception : public std::runtime_error {
public:
	runtime_exception(const std::string& message, position pos)
		: std::runtime_error(message), position(pos) {}

	position position;
};