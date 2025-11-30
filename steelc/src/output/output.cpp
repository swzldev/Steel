#include "output.h"

#include <string>
#include <ostream>
#include <iostream>

#include "log_file.h"

bool output::log_to_file(const std::string& file_path) {
    if (log != nullptr) return false;

	log = new log_file();
	return log->open(file_path);
}

void output::shutdown() {
	if (log != nullptr) {
		delete log;
		log = nullptr;
	}
}

log_file* output::log = nullptr;

void output::write(std::ostream& stream, const std::string& message) {
	stream << message;
	if (log != nullptr) {
		log->write(message);
	}
}
