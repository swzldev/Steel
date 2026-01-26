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
	mm.meta.src_relpath = unit->source_file->relative_path;
	mm.name = unit->source_file->name();
	
	lower_functions(unit->declarations, mm);

	return mm;
}

void mir_lowerer::lower_functions(const std::vector<ast_ptr>& decls, mir_module& mm) {
	for (auto& decl : decls) {
		if (auto mod = ast_ptr_cast<module_declaration>(decl)) {
			lower_functions(mod->declarations, mm);
		}
		else if (auto func = ast_ptr_cast<function_declaration>(decl)) {
			if (func->is_generic && !func->is_generic_instance) {
				continue; // skip generic function templates
			}

			mm.functions.push_back(lower_func(func));
		}
	}
}

mir_function mir_lowerer::lower_func(std::shared_ptr<function_declaration> func) {
	if (func->is_generic && !func->is_generic_instance) {
		throw std::runtime_error("Cannot lower generic function templates");
	}

	mir_function mf;

	// naming
	mf.name = func->identifier;
	if (func->parent_module && !func->parent_module->is_global()) {
		mf.scopes = std::move(func->parent_module->name_path());
	}

	// params
	mf.return_type = { func->return_type };
	mf.params.reserve(func->parameters.size());
	for (const auto& param : func->parameters) {
		// create values for the parameters here to be used later
		auto pval = mf.make_value({ param->type }, param->identifier);
		mf.params.push_back(mir_function_param{ param->type, param->identifier, pval});
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
