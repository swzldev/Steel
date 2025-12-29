#pragma once

#include <vector>

#include <codegen/codegen_config.h>
#include <codegen/code_artifact.h>
#include <linking/link_config.h>
#include <linking/link_result.h>

// icode_linker
//
// interface for intermediate code linkers
//
// note:
// artifacts passed to link will have already been filtered
// to the current backend and format, however it should still
// validate just in case.

class icode_linker {
public:
	virtual ~icode_linker() = default;

	virtual bool can_emit_executable() = 0;
	virtual bool can_emit_static_lib() = 0;
	virtual bool can_emit_shared_lib() = 0;

	virtual link_result link(const std::vector<code_artifact>& artifacts, const codegen_config& cfg) = 0;

	virtual bool supports(const code_artifact& artifact) = 0;
};