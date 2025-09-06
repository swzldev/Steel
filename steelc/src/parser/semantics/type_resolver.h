#pragma once

#include <string>
#include <map>

#include "../ast/ast_visitor.h"
#include "../ast/compilation_unit.h"
#include "../compilation_pass.h"
#include "../symbolics/symbol_resolver.h"
#include "../modules/module_manager.h"

class type_resolver : public ast_visitor, public compilation_pass {
public:
	type_resolver(std::shared_ptr<compilation_unit> unit, module_manager& module_manager)
		: module_manager(module_manager), compilation_pass(unit) {
		resolver.import_tbl = std::make_shared<import_table>(unit->import_tbl);
		resolver.current_module = module_manager.get_global_module();
	}

	void visit(std::shared_ptr<function_declaration> func) override;
	void visit(std::shared_ptr<constructor_declaration> constructor) override;
	void visit(std::shared_ptr<variable_declaration> var) override;
	void visit(std::shared_ptr<type_declaration> decl) override;
	void visit(std::shared_ptr<module_declaration> decl) override;
	void visit(std::shared_ptr<function_call> func_call) override;
	void visit(std::shared_ptr<constructor_call> constructor_call) override;

private:
	symbol_resolver resolver;
	module_manager& module_manager;

	void resolve_type(type_ptr& type, ast_ptr resolvee);
};