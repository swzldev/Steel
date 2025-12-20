#include "code_outputter.h"

#include <filesystem>
#include <fstream>
#include <string>

code_output_error code_outputter::output_code(const std::vector<uint8_t>& code, const std::string& filename, code_output_location location, code_output_format) {
	switch (location) {
	case OUTPUT_LOCATION_OUTPUT:
		return output_to(std::string(code.begin(), code.end()), output_dir / filename, true);
	case OUTPUT_LOCATION_INTERMEDIATE:
		return output_to(std::string(code.begin(), code.end()), intermediate_dir / filename, true);
	}
	return OUTPUT_SUCCESS;
}

code_output_error code_outputter::output_code(const std::string& code, const std::string& filename, code_output_location location, code_output_format format) {
	switch (location) {
	case OUTPUT_LOCATION_OUTPUT:
		return output_to(code, output_dir / filename, format == OUTPUT_FORMAT_BINARY);
	case OUTPUT_LOCATION_INTERMEDIATE:
		return output_to(code, intermediate_dir / filename, format == OUTPUT_FORMAT_BINARY);
	}

	return OUTPUT_SUCCESS;
}

void code_outputter::clear_intermediate_files(const std::string& subpath) const {
	if (subpath.empty()) {
		std::filesystem::remove_all(intermediate_dir);
		std::filesystem::create_directories(intermediate_dir);
	}
	else {
		std::filesystem::remove_all(intermediate_dir / subpath);
		std::filesystem::create_directories(intermediate_dir / subpath);
	}
}

bool code_outputter::init() {
	output_dir = project_dir / build_cfg.output_dir;
	intermediate_dir = project_dir / build_cfg.intermediate_dir;

	// create dirs
	std::filesystem::create_directories(output_dir);
	std::filesystem::create_directories(intermediate_dir);

	if (!std::filesystem::exists(output_dir) || !std::filesystem::exists(intermediate_dir)) {
		// failed to create output or intermediate dir
		return false;
	}

	return true;
}
code_output_error code_outputter::output_to(const std::string& data, const std::filesystem::path& path, bool binary) {
	// create directories (if nescessary)
	std::filesystem::create_directories(path.parent_path());

	std::ofstream file;
	if (binary) {
		file.open(path, std::ios::out | std::ios::binary);
	}
	else {
		file.open(path, std::ios::out);
	}

	if (!file || !file.is_open()) {
		return OUTPUT_FAIL_CREATE_FILE;
	}

	if (binary) {
		file.write(data.data(), static_cast<std::streamsize>(data.size()));
	}
	else {
		file << data;
	}

	file.close();

	return OUTPUT_SUCCESS;
}
