#pragma once

#include <exception>
#include <string>

#define cg_assert(cond, msg) \
	if (!(cond)) { \
		throw codegen_exception(msg); \
	}

class codegen_exception : public std::exception {
public:
	codegen_exception(const std::string& err)
		: msg(msg) {
	}

	virtual const char* what() const noexcept override {
		return msg.c_str();
	}

private:
    std::string msg;
};