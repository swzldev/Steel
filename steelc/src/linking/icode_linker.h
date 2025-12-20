#pragma once

#include <vector>

#include <codegen/code_artifact.h>

class icode_linker {
public:
	virtual ~icode_linker() = default;

	virtual code_artifact link(const std::vector<code_artifact>& artifacts) = 0;

	virtual bool supports(const code_artifact& artifact) = 0;
};