#include "log_file.h"

#include <string>
#include <fstream>

log_file::~log_file() {
	if (file_stream.is_open()) {
		file_stream.close();
	}
}

bool log_file::open(const std::string& file_path) {
	file_stream = std::ofstream(file_path, std::ios::app);
	return file_stream.is_open();
}

void log_file::write(const std::string& message) {
	if (file_stream.is_open()) {
		file_stream << message;
	}
}
