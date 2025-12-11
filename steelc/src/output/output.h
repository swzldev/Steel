#pragma once

#include <string>
#include <format>
#include <ostream>
#include <iostream>

#include <output/log_file.h>
#include <utils/console_colors.h>

enum log_verbosity {
	LOG_VERBOSITY_NORMAL,
	LOG_VERBOSITY_HIGH,
};

class output {
public:
	template<typename... Args>
	static void print(const std::string& message, const std::string& style = "", Args&&... args) {
		write_va(std::cout, style + message, std::forward<Args>(args)...);
	}
	template<typename... Args>
	static void verbose(const std::string& message, const std::string& style = console_colors::DIM, Args&&... args) {
		if (current_verbosity >= LOG_VERBOSITY_HIGH) {
			write_va(std::cout, style + message, std::forward<Args>(args)...);
		}
	}
	template<typename... Args>
	static void err(const std::string& err, const std::string& style = console_colors::RED, Args&&... args) {
		write_va(std::cerr, style + err, std::forward<Args>(args)...);
	}

	static bool log_to_file(const std::string& file_path);

	static void set_log_verbosity(log_verbosity verbosity);

	static void init();
	// should always be called before program termination
	static void shutdown();

private:
	output() = delete;
	~output() = delete;
	output(const output&) = delete;
	output& operator=(const output&) = delete;

	static log_file* log;
	static log_verbosity current_verbosity;

	template<typename... Args>
	static void write_va(std::ostream& stream, const std::string& message, Args&&... args) {
		std::string fmt = std::vformat(message, std::make_format_args(args...));
		write(stream, fmt + "\033[0m");
	}

	static void write(std::ostream& stream, const std::string& message);
};