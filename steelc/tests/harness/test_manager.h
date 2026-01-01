#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>

#include <harness/test.h>

// test_manager
//
// this class is responsible for managing and executing tests
// as well as logging results

struct run_results {
	size_t total_tests = 0;
	std::vector<std::string> passed_tests = {};
	std::map<std::string, int> failed_tests = {};
};

class test_manager {
public:
	static run_results run_all_tests(bool display_output = true);

	static void register_test(const test& test);

private:
	static std::map<std::string, test> tests_map;

	static void test_succeeded(const std::string& test_name);
	static void test_failed(const std::string& test_name, int error_code);
};

struct test_registrar {
	inline test_registrar(const std::string& test_name, const std::function<int()>& test_func) {
		test_manager::register_test(test(test_name, test_func));
	}
};
