#include "code_outputter.h"

#include <filesystem>
#include <fstream>
#include <string>

code_output_error code_outputter::output_il(const std::string& il, const std::string& filename) {
	return output_code(il, filename, OUTPUT_LOCATION_INTERMEDIATE);
}
code_output_error code_outputter::output_code(const std::string& code, const std::string& filename, code_output_location location) {
	if (!init) {
		init_structure();
		if (!init) {
			return OUTPUT_FAIL_INIT;
		}
	}
	
	switch (location) {
	case OUTPUT_LOCATION_OUTPUT:
		return output_to(code, output_dir / filename);
	case OUTPUT_LOCATION_INTERMEDIATE:
		return output_to(code, intermediate_dir / filename);
	}

	return OUTPUT_SUCCESS;
}

void code_outputter::init_structure() {
	if (init) return;

	output_dir = project_dir / config.output_dir;
	intermediate_dir = project_dir / config.intermediate_dir;

	// create dirs
	std::filesystem::create_directories(output_dir);
	std::filesystem::create_directories(intermediate_dir);

	init = true;
}
code_output_error code_outputter::output_to(const std::string& data, const std::filesystem::path& path) {
	// create directories (if nescessary)
	std::filesystem::create_directories(path.parent_path());

	std::ofstream file(path);
	if (!file.is_open()) {
		return OUTPUT_FAIL_CREATE_FILE;
	}

	file << data;
	file.close();

	return OUTPUT_SUCCESS;
}
