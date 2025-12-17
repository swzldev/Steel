#include "mir_lowerer.h"

#include <memory>

#include <ast/ast.h>
#include <mir/mir_module.h>
#include <mir/mir_function.h>
#include <mir/mir_block.h>
#include <ast_lowering/mir_lowering_visitor.h>
#include <representations/entities/module_entity.h>

mir_module mir_lowerer::lower_unit(std::shared_ptr<compilation_unit> unit) {
	mir_module mm;
	for (auto& decl : unit->declarations) {
		if (auto fn = std::dynamic_pointer_cast<function_declaration>(decl)) {
			if (fn->is_generic && !fn->is_generic_instance) {
				// skip generic function templates
				continue;
			}
			mm.functions.push_back(lower_func(fn));
		}
	}
	return mm;
}

mir_function mir_lowerer::lower_func(std::shared_ptr<function_declaration> func) {
	mir_function mf;

	// naming
	mf.name = func->identifier;
	if (func->parent_module && !func->parent_module->is_global()) {
		mf.scopes = std::move(func->parent_module->name_path());
	}

	// types
	mf.return_type = { func->return_type };
	mf.param_types.reserve(func->parameters.size());
	for (const auto& param : func->parameters) {
		mf.param_types.push_back({ param->type });
	}

	// entry block
	{
		mir_block entry_block{ "entry" };
		mf.blocks.push_back(std::move(entry_block));
	}

	// body - use lowering visitor for statements
	mir_lowering_visitor visitor(mf);
	func->accept(visitor);

	return mf;
}
