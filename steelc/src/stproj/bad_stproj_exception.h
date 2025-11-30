#pragma once

#include <exception>
#include <string>

class bad_stproj_exception : public std::exception {
public:
	bad_stproj_exception(const std::string& message)
		: msg(message) {
	}

	virtual const char* what() const noexcept override {
		return msg.c_str();
	}
	const std::string& message() const {
		return msg;
	}

private:
	std::string msg;
};