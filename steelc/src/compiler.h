#pragma once

#include <vector>
#include <string>
#include <memory>

#include "error/error.h"
#include "config/compile_config.h"
#include "parser/ast/compilation_unit.h"
#include "parser/modules/module_manager.h"

class source_file;

class compiler {
public:
	compiler(std::vector<source_file> sources)
		: sources(sources) {
	}

	bool compile(compile_config cfg);

	inline bool has_errors() const {
		return !errors.empty();
	}
	inline bool has_warnings() const {
		return !warnings.empty();
	}

	inline const std::vector<error>& get_errors() const {
		return errors;
	}
	inline const std::vector<error>& get_warnings() const {
		return warnings;
	}

	std::vector<std::shared_ptr<compilation_unit>> compilation_units;

	std::vector<error> errors;
	std::vector<error> warnings;

	module_manager module_manager;
private:
	std::vector<source_file> sources;

	std::vector<std::string> read_source(std::string& path);
};