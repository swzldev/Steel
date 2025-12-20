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

	void merge(const codegen_result& result) {
		artifacts.insert(
			artifacts.end(),
			result.artifacts.begin(),
			result.artifacts.end()
		);
		diagnostics.insert(
			diagnostics.end(),
			result.diagnostics.begin(),
			result.diagnostics.end()
		);
		if (!result.success) {
			success = false;
		}
	}
};