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

	void visit(std::shared_ptr<class variable_declaration> var) override;
	void visit(std::shared_ptr<class binary_expression> expr) override;
	void visit(std::shared_ptr<class assignment_expression> expr) override;
	void visit(std::shared_ptr<class unary_expression> expr) override;
	void visit(std::shared_ptr<class index_expression> expr) override;
	void visit(std::shared_ptr<class cast_expression> expr) override;
	void visit(std::shared_ptr<class function_call> func_call) override;
	void visit(std::shared_ptr<class constructor_call> constructor_call) override;
	void visit(std::shared_ptr<class if_statement> if_stmt) override;
	void visit(std::shared_ptr<class for_loop> for_loop) override;
	void visit(std::shared_ptr<class while_loop> while_loop) override;

private:
	bool is_valid_conversion(type_ptr from, type_ptr to, bool implicit, position pos);
};