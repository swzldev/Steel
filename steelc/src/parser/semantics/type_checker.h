#pragma once

#include <string>
#include <map>

#include "../ast/ast_visitor.h"
#include "../compilation_pass.h"
#include "../symbolics/symbol_table.h"

class type_checker : public ast_visitor, public compilation_pass {
public:
	type_checker(std::shared_ptr<compilation_unit> unit)
		: compilation_pass(unit) {
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
	void visit(std::shared_ptr<return_if> ret_stmt) override;

private:
	std::shared_ptr<function_declaration> current_function = nullptr;
	std::vector<std::map<std::string, type_ptr>> gtm_stack;

	struct candidate_score {
		std::shared_ptr<function_declaration> candidate;
		int score = 0;
	};

	std::shared_ptr<custom_type> member_access_allowed(type_ptr type);
	bool is_valid_conversion(type_ptr from, type_ptr to, bool implicit, position pos);
	bool is_valid_upcast(type_ptr from, type_ptr to, position pos);

	int score_candidate(std::shared_ptr<function_declaration> candidate, const std::vector<type_ptr>& arg_types);

	std::shared_ptr<function_declaration> unbox_generic_func(std::shared_ptr<function_declaration> func, std::vector<type_ptr> types);
};