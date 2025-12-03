#include "output.h"

#include <string>
#include <ostream>
#include <iostream>

#include "log_file.h"
#include "../utils/console_colors.h"

bool output::log_to_file(const std::string& file_path) {
    if (log != nullptr) return false;

	log = new log_file();
	return log->open(file_path);
}

void output::set_log_verbosity(log_verbosity verbosity) {
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
	current_verbosity = verbosity < LOG_VERBOSITY_HIGH ? LOG_VERBOSITY_HIGH : verbosity;
#else
	current_verbosity = verbosity;
#endif
}

void output::init() {
	console_colors::enable();
}
void output::shutdown() {
	if (log != nullptr) {
		delete log;
		log = nullptr;
	}
}

log_file* output::log = nullptr;

#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
log_verbosity output::current_verbosity = LOG_VERBOSITY_HIGH;
#else
log_verbosity output::current_verbosity = LOG_VERBOSITY_NORMAL;
#endif

void output::write(std::ostream& stream, const std::string& message) {
	stream << message;
	if (log != nullptr) {
		log->write(message);
	}
}
