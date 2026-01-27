#pragma once

#include <string>
#include <map>
#include <memory>
#include <unordered_set>

#include <ast/ast_visitor.h>
#include <compiler/compilation_pass.h>
#include <compiler/compilation_ctx.h>
#include <modules/module_manager.h>
#include <symbolics/symbol_table.h>
#include <representations/entities/module_entity.h>

class declaration_collector : public ast_visitor, public compilation_pass {
public:
	declaration_collector(std::shared_ptr<compilation_unit> unit, compilation_ctx& ctx)
		: unit(unit), module_manager(ctx.module_manager), sym_table(&ctx.module_manager.get_global_module()->symbols()), compilation_pass(unit) {
		current_module = module_manager.get_global_module();
	}

	// declarations
	void visit(std::shared_ptr<function_declaration> func) override;
	void visit(std::shared_ptr<variable_declaration> var) override;
	void visit(std::shared_ptr<type_declaration> decl) override;
	void visit(std::shared_ptr<module_declaration> mod) override;
	void visit(std::shared_ptr<enum_declaration> enum_decl) override;

	// top-level statements
	void visit(std::shared_ptr<import_statement> import_stmt) override;

private:
	std::shared_ptr<compilation_unit> unit;
	symbol_table* sym_table;
	module_manager& module_manager;

	std::shared_ptr<function_declaration> current_function;
	std::shared_ptr<function_declaration> current_constructor;
	std::shared_ptr<type_declaration> current_type;
	std::shared_ptr<module_entity> current_module; // should never be null - at least the global module
};