#pragma once

#include <vector>
#include <string>
#include <memory>

#include "error/error.h"
#include "parser/ast/compilation_unit.h"
#include "parser/modules/module_manager.h"

class compiler {
public:
	compiler() = default;

	bool compile(std::vector<class source_file> sources);

	inline bool has_errors() const {
		return !lexer_errors.empty() || !parsing_errors.empty() || !semantic_errors.empty();
	}

	std::vector<std::shared_ptr<compilation_unit>> compilation_units;

	std::vector<error> lexer_errors;
	std::vector<error> parsing_errors;
	std::vector<error> semantic_errors;

	module_manager module_manager;
private:
	std::vector<std::string> read_source(std::string& path);
};