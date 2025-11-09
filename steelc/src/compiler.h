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
		return !get_errors().empty();
	}
	inline std::vector<error> get_errors() const {
		std::vector<error> errors;
		for (const auto& err : lexer_errors) {
			if (err.type == ERR_ERROR) {
				errors.push_back(err);
			}
		}
		for (const auto& err : parsing_errors) {
			if (err.type == ERR_ERROR) {
				errors.push_back(err);
			}
		}
		for (const auto& err : semantic_errors) {
			if (err.type == ERR_ERROR) {
				errors.push_back(err);
			}
		}
		return errors;
	}
	inline std::vector<error> get_warnings() const {
		std::vector<error> warnings;
		for (const auto& wrn : lexer_errors) {
			if (wrn.type == ERR_WARNING) {
				warnings.push_back(wrn);
			}
		}
		for (const auto& wrn : parsing_errors) {
			if (wrn.type == ERR_WARNING) {
				warnings.push_back(wrn);
			}
		}
		for (const auto& wrn : semantic_errors) {
			if (wrn.type == ERR_WARNING) {
				warnings.push_back(wrn);
			}
		}
		return warnings;
	}

	std::vector<std::shared_ptr<compilation_unit>> compilation_units;

	std::vector<error> lexer_errors;
	std::vector<error> parsing_errors;
	std::vector<error> semantic_errors;

	module_manager module_manager;
private:
	std::vector<source_file> sources;

	std::vector<std::string> read_source(std::string& path);
};