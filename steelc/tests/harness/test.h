#pragma once

#include <string>
#include <functional>

struct test {
	inline test(const std::string& test_name, const std::function<int()>& test_func)
		: name(test_name), func(test_func) {
	}

	inline int run() const {
		return func();
	}

	std::string name;
	std::function<int()> func;
};