#pragma once

#include <memory>
#include <vector>
#include <string>

#include <codegen/codegen_result.h>
#include <ast/ast_fwd.h>
#include <modules/module_manager.h>

class codegen {
public:
	codegen(module_manager& module_manager, std::vector<std::shared_ptr<compilation_unit>> units)
		: module_manager(module_manager), compilation_units(units) {
	}

	std::unique_ptr<codegen_result> generate_modules();

private:
	module_manager& module_manager;
	std::vector<std::shared_ptr<compilation_unit>> compilation_units;
	std::unordered_set<std::string> llvm_module_names;

	std::string get_module_name(std::shared_ptr<compilation_unit> unit);
};