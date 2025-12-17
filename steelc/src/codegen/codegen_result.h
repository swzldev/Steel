#pragma once

#include <memory>
#include <vector>

#include <codegen/code_artifact.h>

struct codegen_diag {
	// TODO
};

struct codegen_result {
	std::vector<code_artifact> artifacts;
	std::vector<codegen_diag> diagnostics;
	bool success = true;
};