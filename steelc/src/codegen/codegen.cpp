#include "codegen.h"


#include <string>
#include <vector>
#include <iostream>

#include "codegen_visitor.h"
#include "error/codegen_exception.h"

std::vector<llvm_ir_holder> codegen::generate_ir() {
	std::vector<llvm_ir_holder> ir_modules;

	for (const auto& unit : compilation_units) {
		std::string module_name = get_module_name(unit);
		codegen_visitor visitor(module_name);
		try {
			for (const auto& decl : unit->declarations) {
				decl->accept(visitor);
			}
		}
		catch (const codegen_exception& e) {
			std::cerr << "Codegen error in module " << module_name << ": " << e.what() << std::endl;
			continue;
		}

		// extract IR from module
		llvm::Module& module = visitor.get_module();
		llvm_ir_holder ir_holder;
		ir_holder.owning_unit = unit;
		std::string ir_string;
		llvm::raw_string_ostream rso(ir_string);
		module.print(rso, nullptr);
		ir_holder.ir = rso.str();
		ir_modules.push_back(ir_holder);
	}

	return ir_modules;
}

std::string codegen::get_module_name(std::shared_ptr<compilation_unit> unit) {
	std::string name = unit->source_file->name();

	size_t i = 1;
	while (llvm_module_names.find(name) != llvm_module_names.end()) {
		name = unit->source_file->name() + "_" + std::to_string(i);
	}

	llvm_module_names.insert(name);
	return name;
}
