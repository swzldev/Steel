#pragma once

#include <string>
#include <map>

#include "../ast/ast_visitor.h"
#include "../compilation_pass.h"
#include "../symbolics/symbol_table.h"

class type_checker : public ast_visitor, public compilation_pass {
public:
	type_checker(std::shared_ptr<class compilation_unit> unit)
		: compilation_pass(unit) {
	}

	void visit(std::shared_ptr<variable_declaration> var) override;
	void visit(std::shared_ptr<type_declaration> decl) override;
	void visit(std::shared_ptr<binary_expression> expr) override;
	void visit(std::shared_ptr<assignment_expression> expr) override;
	void visit(std::shared_ptr<address_of_expression> expr) override;
	void visit(std::shared_ptr<unary_expression> expr) override;
	void visit(std::shared_ptr<index_expression> expr) override;
	void visit(std::shared_ptr<cast_expression> expr) override;
	void visit(std::shared_ptr<function_call> func_call) override;
	void visit(std::shared_ptr<if_statement> if_stmt) override;
	void visit(std::shared_ptr<inline_if> inline_if) override;
	void visit(std::shared_ptr<for_loop> for_loop) override;
	void visit(std::shared_ptr<while_loop> while_loop) override;

private:
	bool is_valid_conversion(type_ptr from, type_ptr to, bool implicit, position pos);
};