#pragma once

#include <vector>
#include <optional>

#include <codegen/code_artifact.h>

struct link_error {
	std::string message;
};

struct link_result {
	link_result() = default;
	link_result(link_error err)
		: success(false), error(err) {
	}

	bool success = true;
	link_error error;

	std::string final_executable_path;
	//std::vector<code_artifact> additional; // for now unused
};