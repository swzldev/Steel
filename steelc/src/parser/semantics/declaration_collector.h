#pragma once

#include <string>
#include <map>
#include <memory>
#include <unordered_set>

#include "../ast/ast_visitor.h"
#include "../compilation_pass.h"
#include "../modules/module_manager.h"

class declaration_collector : public ast_visitor, public compilation_pass {
public:
	declaration_collector(std::shared_ptr<compilation_unit> unit, module_manager& module_manager)
		: module_manager(module_manager), sym_table(&module_manager.get_global_module()->symbols), compilation_pass(unit) {
		current_module = module_manager.get_global_module();
	}

	// declarations
	void visit(std::shared_ptr<class function_declaration> func) override;
	void visit(std::shared_ptr<class constructor_declaration> constructor) override;
	void visit(std::shared_ptr<class variable_declaration> var) override;
	void visit(std::shared_ptr<class type_declaration> decl) override;
	void visit(std::shared_ptr<class module_declaration> mod) override;

	// top-level statements
	void visit(std::shared_ptr<class import_statement> import_stmt) override;

private:
	symbol_table* sym_table;
	module_manager& module_manager;

	std::shared_ptr<function_declaration> current_function;
	std::shared_ptr<constructor_declaration> current_constructor;
	std::shared_ptr<type_declaration> current_type;
	std::shared_ptr<module_info> current_module;
};