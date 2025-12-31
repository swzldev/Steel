#pragma once

#include <vector>
#include <string>

#include <codegen/codegen_config.h>
#include <linking/link_result.h>

// icode_linker
//
// interface for intermediate code linkers

class icode_linker {
public:
	virtual ~icode_linker() = default;

	virtual bool can_emit_executable() = 0;
	virtual bool can_emit_static_lib() = 0;
	virtual bool can_emit_shared_lib() = 0;

	virtual link_result link(const std::vector<std::string>& to_link, const codegen_config& cfg) = 0;
};