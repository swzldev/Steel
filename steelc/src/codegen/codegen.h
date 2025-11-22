#pragma once

#include <memory>
#include <vector>

#include "ir/llvm_ir_holder.h"
#include "../parser/ast/ast.h"
#include "../parser/modules/module_manager.h"

class codegen {
public:
	codegen(module_manager& module_manager, std::vector<std::shared_ptr<compilation_unit>> units)
		: module_manager(module_manager), compilation_units(units) {
	}

	std::vector<llvm_ir_holder> generate_ir();

private:
	module_manager& module_manager;
	std::vector<std::shared_ptr<compilation_unit>> compilation_units;
	std::unordered_set<std::string> llvm_module_names;

	std::string get_module_name(std::shared_ptr<compilation_unit> unit);
};