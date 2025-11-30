#pragma once

#include <string>
#include <fstream>

class log_file {
public:
	log_file() = default;
	~log_file();

	bool open(const std::string& file_path);
	void write(const std::string& message);

private:
	std::string file_path;
	std::ofstream file_stream;
};