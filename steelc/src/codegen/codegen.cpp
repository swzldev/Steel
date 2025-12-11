#include "codegen.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <ast/compilation_unit.h>
#include <codegen/codegen_visitor.h>
#include <codegen/error/codegen_exception.h>
#include <codegen/codegen_result.h>
#include <output/output.h>

std::unique_ptr<codegen_result> codegen::generate_modules() {
	auto result = std::make_unique<codegen_result>(std::make_unique<llvm::LLVMContext>());

	for (const auto& unit : compilation_units) {
		std::string module_name = get_module_name(unit);
		module_holder mod_holder(module_name, *result->context, unit);

		codegen_visitor visitor(*mod_holder.module);
		try {
			unit->accept(visitor);
		}
		catch (const codegen_exception& e) {
			output::err("Codegen error in module {}: {}\n", console_colors::RED, module_name, e.message());
			continue;
		}

		result->modules.push_back(std::move(mod_holder));
	}

	return result;
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
