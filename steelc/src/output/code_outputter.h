#pragma once

#include <string>
#include <filesystem>

#include "output_config.h"

enum code_output_location {
	OUTPUT_LOCATION_INTERMEDIATE,
	OUTPUT_LOCATION_OUTPUT
};

enum code_output_error {
	OUTPUT_SUCCESS = 0,
	OUTPUT_FAIL_INIT,
	OUTPUT_FAIL_CREATE_FILE,
};

class code_outputter {
public:
	code_outputter(std::string project_dir, output_config cfg)
		: project_dir(project_dir), config(cfg) {
	}

	code_output_error output_il(const std::string& il, const std::string& filename);
	code_output_error output_code(const std::string& code, const std::string& filename, code_output_location location);

	inline std::filesystem::path get_output_dir() const {
		return output_dir;
	}
	inline std::filesystem::path get_intermediate_dir() const {
		return intermediate_dir;
	}

private:
	std::filesystem::path project_dir;
	std::filesystem::path output_dir;
	std::filesystem::path intermediate_dir;
	output_config config;
	bool init = false;

	void init_structure();
	code_output_error output_to(const std::string& data, const std::filesystem::path& path);
};