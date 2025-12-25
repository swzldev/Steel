#pragma once

#include <vector>
#include <optional>

#include <codegen/code_artifact.h>

struct link_result {
	code_artifact final_intermediate; // must be an object file
	std::vector<code_artifact> extras;
};