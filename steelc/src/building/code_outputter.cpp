#include "code_outputter.h"

#include <filesystem>
#include <fstream>
#include <string>

code_output_error code_outputter::output_code(const std::vector<uint8_t>& bytes, const std::string& filename) {
	std::string data(bytes.begin(), bytes.end());
	return output_to(data, project_dir / filename, true);
}
code_output_error code_outputter::output_code(const std::string& text, const std::string& filename) {
	return output_to(text, project_dir / filename, false);
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
