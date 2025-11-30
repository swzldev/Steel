#pragma once

#include <string>
#include <format>
#include <ostream>
#include <iostream>

#include "log_file.h"

class output {
public:
	template<typename... Args>
	static void print(const std::string& message, const std::string& style = "", Args&&... args) {
		write_va(std::cout, style + message, std::forward<Args>(args)...);
	}
	template<typename... Args>
	static void err(const std::string& err, const std::string& style = "", Args&&... args) {
		write_va(std::cerr, style + err, std::forward<Args>(args)...);
	}

	static bool log_to_file(const std::string& file_path);

	// should always be called before program termination
	static void shutdown();

private:
	output() = delete;
	~output() = delete;
	output(const output&) = delete;
	output& operator=(const output&) = delete;

	static log_file* log;

	template<typename... Args>
	static void write_va(std::ostream& stream, const std::string& message, Args&&... args) {
		std::string fmt = std::vformat(message, std::make_format_args(args...));
		write(stream, fmt + "\033[0m");
	}

	static void write(std::ostream& stream, const std::string& message);
};