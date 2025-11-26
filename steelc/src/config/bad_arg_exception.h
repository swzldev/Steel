#pragma once

#include <exception>
#include <string>

class bad_arg_exception : public std::exception {
public:
	bad_arg_exception(const std::string& msg) : message(msg) {}

	const char* what() const noexcept override {
		return message.c_str();
	}

private:
	std::string message;
};