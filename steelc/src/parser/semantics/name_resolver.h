#pragma once

#include <string>
#include <map>
#include <memory>

#include "../ast/ast_visitor.h"
#include "../ast/compilation_unit.h"
#include "../compilation_pass.h"
#include "../symbolics/symbol_table.h"
#include "../symbolics/symbol_resolver.h"
#include "../modules/module_manager.h"
#include "../entities/module_entity.h"
#include "../entities/type_entity.h"

class name_resolver : public ast_visitor, public compilation_pass {
public:
	name_resolver(std::shared_ptr<compilation_unit> unit, module_manager& module_manager)
		: module_manager(module_manager), compilation_pass(unit) {
		resolver.import_tbl = std::make_shared<import_table>(unit->import_tbl);
		resolver.current_module = module_manager.get_global_module();
		sym_table = &module_manager.get_global_module()->symbols();
	}

	void visit(std::shared_ptr<function_declaration> func) override;
	void visit(std::shared_ptr<variable_declaration> var) override;
	void visit(std::shared_ptr<type_declaration> decl) override;
	void visit(std::shared_ptr<module_declaration> module) override;
	void visit(std::shared_ptr<binary_expression> expr) override;
	void visit(std::shared_ptr<identifier_expression> expr) override;
	void visit(std::shared_ptr<member_expression> expr) override;
	void visit(std::shared_ptr<this_expression> expr) override;
	void visit(std::shared_ptr<function_call> func_call) override;
	void visit(std::shared_ptr<code_block> block) override;
	void visit(std::shared_ptr<if_statement> if_stmt) override;
	void visit(std::shared_ptr<for_loop> for_loop) override;
	void visit(std::shared_ptr<while_loop> while_loop) override;

private:
	symbol_resolver resolver;
	module_manager& module_manager;
	symbol_table* sym_table;

	std::shared_ptr<function_declaration> current_func;
	std::shared_ptr<function_declaration> current_ctor;
	std::shared_ptr<type_declaration> current_type;

	// helper that returns nullptr if no current type
	std::shared_ptr<type_entity> current_type_entity() const;

	void resolve_type_names(type_ptr& type);
};