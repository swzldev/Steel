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
	return false;
}
int console_args::find_arg(const std::string& arg) const {
	for (int i = 0; i < args.size(); i++) {
		if (arg == args[i]) {
			return i;
		}
	}
	return -1;
}
