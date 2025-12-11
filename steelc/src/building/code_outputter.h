#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include <vector>
#include <cstdint>

#include <building/build_config.h>

enum code_output_location {
	OUTPUT_LOCATION_INTERMEDIATE,
	OUTPUT_LOCATION_OUTPUT
};

enum code_output_format {
	OUTPUT_FORMAT_BINARY,
	OUTPUT_FORMAT_TEXT
};

enum code_output_error {
	OUTPUT_SUCCESS = 0,
	OUTPUT_FAIL_INIT,
	OUTPUT_FAIL_CREATE_FILE,
};

class code_outputter {
public:
	// returns nullptr if failed to init
	static std::unique_ptr<code_outputter> create(const std::string& project_dir, build_config cfg) {
		auto outputter = std::unique_ptr<code_outputter>(new code_outputter(project_dir, cfg));
		if (!outputter->init()) {
			return nullptr;
		}
		return outputter;
	}

	code_output_error output_code(const std::string& code, const std::string& filename, code_output_location location, code_output_format = OUTPUT_FORMAT_TEXT);

	void clear_intermediate_files(const std::string& subpath = "") const;

	inline std::filesystem::path get_output_dir() const {
		return output_dir;
	}
	inline std::filesystem::path get_intermediate_dir() const {
		return intermediate_dir;
	}

private:
	code_outputter(std::string project_dir, build_config cfg)
		: project_dir(project_dir), build_cfg(cfg) {
	}

	std::filesystem::path project_dir;
	std::filesystem::path output_dir;
	std::filesystem::path intermediate_dir;
	build_config build_cfg;

	bool init();
	code_output_error output_to(const std::string& data, const std::filesystem::path& path, bool binary);
};