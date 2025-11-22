#include "console_args.h"

#include <string>

console_args::console_args(int argc, char** argv) {
	for (int i = 0; i < argc; i++) {
		args.push_back(argv[i]);
	}
}
bool console_args::has_arg(const std::string& arg) const {
	for (const auto& a : args) {
		if (a == arg) {
			return true;
		}
	}
}