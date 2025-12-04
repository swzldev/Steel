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

console_args console_args::slice(size_t start, size_t end) const {
	console_args sliced(0, nullptr);
	if (start >= args.size() || end > args.size() || start >= end) {
		return sliced;
	}
	sliced.args.insert(sliced.args.end(), args.begin() + start, args.begin() + end);
	return sliced;
}
console_args console_args::slice(size_t start) const {
	console_args sliced(0, nullptr);
	if (start >= args.size()) {
		return sliced;
	}
	sliced.args.insert(sliced.args.end(), args.begin() + start, args.end());
	return sliced;
}
