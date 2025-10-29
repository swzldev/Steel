#pragma once

#include <string>
#include <map>

#include "../ast/ast_visitor.h"
#include "../compilation_pass.h"
#include "../symbolics/symbol_table.h"
#include "../modules/module_manager.h"

class type_checker : public ast_visitor, public compilation_pass {
public:
	type_checker(std::shared_ptr<compilation_unit> unit, module_manager& module_manager)
		: compilation_pass(unit), module_manager(module_manager) {
	}

	void visit(std::shared_ptr<function_declaration> func) override;
	void visit(std::shared_ptr<variable_declaration> var) override;
	void visit(std::shared_ptr<type_declaration> decl) override;
	void visit(std::shared_ptr<identifier_expression> expr) override;
	void visit(std::shared_ptr<binary_expression> expr) override;
	void visit(std::shared_ptr<assignment_expression> expr) override;
	void visit(std::shared_ptr<address_of_expression> expr) override;
	void visit(std::shared_ptr<deref_expression> expr) override;
	void visit(std::shared_ptr<unary_expression> expr) override;
	void visit(std::shared_ptr<index_expression> expr) override;
	void visit(std::shared_ptr<cast_expression> expr) override;
	void visit(std::shared_ptr<member_expression> expr) override;
	void visit(std::shared_ptr<initializer_list> init) override;
	void visit(std::shared_ptr<function_call> func_call) override;
	void visit(std::shared_ptr<if_statement> if_stmt) override;
	void visit(std::shared_ptr<inline_if> inline_if) override;
	void visit(std::shared_ptr<for_loop> for_loop) override;
	void visit(std::shared_ptr<while_loop> while_loop) override;
	void visit(std::shared_ptr<return_statement> ret_stmt) override;

private:
	module_manager& module_manager;
	std::shared_ptr<function_declaration> current_function = nullptr;

	std::map<std::shared_ptr<function_declaration>, std::vector<std::shared_ptr<function_declaration>>> generic_function_instances;
	std::map<std::shared_ptr<type_declaration>, std::vector<std::shared_ptr<type_declaration>>> generic_type_instances;
	std::vector<type_ptr> generic_substitutions;

	struct candidate_score {
		std::shared_ptr<function_declaration> candidate;
		int score = 0;
	};

	std::shared_ptr<custom_type> member_access_allowed(type_ptr type);
	bool is_valid_conversion(type_ptr from, type_ptr to, bool implicit, position pos);
	bool is_valid_upcast(type_ptr from, type_ptr to, position pos);
	bool is_valid_entry_point(std::shared_ptr<function_declaration> entry);

	int score_candidate(std::shared_ptr<function_declaration> candidate, const std::vector<type_ptr>& arg_types);

	void try_unbox_type(type_ptr& type);

	std::shared_ptr<function_declaration> unbox_generic_func(std::shared_ptr<function_declaration> generic_func, const std::vector<type_ptr>& types);
	std::shared_ptr<type_declaration> unbox_generic_type(std::shared_ptr<type_declaration> generic_type, const std::vector<type_ptr>& types);
};