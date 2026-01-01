#include "test_manager.h"

#include <iostream>
#include <string>
#include <map>

#include <harness/test.h>

run_results test_manager::run_all_tests(bool display_output) {
	run_results results;

	if (display_output) {
		std::cout << "Running " << tests_map.size() << " tests..." << std::endl;
	}

	for (const auto& [name, test] : tests_map) {
		int result = test.run();
		if (result == 0) {
			if (display_output) test_succeeded(name);
			results.passed_tests.push_back(name);
		}
		else {
			if (display_output) test_failed(name, result);
			results.failed_tests.insert({ name, result });
		}
		results.total_tests++;
	}

	if (display_output) {
		std::cout << "\nTest run complete. "
				  << results.passed_tests.size() << " passed, "
				  << results.failed_tests.size() << " failed."
				  << std::endl;
	}

	return results;
}

void test_manager::register_test(const test& test) {
	tests_map.insert({ test.name, test });
}

std::map<std::string, test> test_manager::tests_map = {};

void test_manager::test_succeeded(const std::string& test_name) {
	std::cout << "[  PASSED  ] " << test_name << std::endl;
}
void test_manager::test_failed(const std::string& test_name, int error_code) {
	std::cout << "[  FAILED  ] " << test_name << " (Error code: " << error_code << ")" << std::endl;
}
