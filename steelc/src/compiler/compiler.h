#pragma once

#include <vector>
#include <string>
#include <memory>

#include <error/error.h>
#include <config/compile_config.h>
#include <ast/compilation_unit.h>
#include <modules/module_manager.h>
#include <codegen/codegen_result.h>
#include <codegen/codegen_config.h>

class source_file;

class compiler {
public:
#if defined(DEBUG) || defined(_DEBUG)
	static constexpr bool DBG_MODE = true;
#else
	static constexpr bool DBG_MODE = false;
#endif

	// these flags are mostly meant for debugging purposes
	static constexpr bool PRINT_SRC = false || DBG_MODE;
	static constexpr bool PRINT_TOKENS = false || DBG_MODE;
	static constexpr bool PRINT_MIR = false || DBG_MODE;

public:
	compiler(std::vector<source_file> sources)
		: sources(sources) {
	}

	bool compile(const compile_config& cl_cfg, codegen_config& cg_cfg);

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

	inline const size_t get_error_count() const {
		return errors.size();
	}
	inline const size_t get_warning_count() const {
		return warnings.size();
	}

	inline const codegen_result& get_result() {
		return codegen_result;
	}

private:
	std::vector<std::shared_ptr<compilation_unit>> compilation_units;

	std::vector<error> errors;
	std::vector<error> warnings;

	module_manager module_manager;

	std::vector<source_file> sources;
	codegen_result codegen_result;

	std::vector<std::string> read_source(std::string& path);
};