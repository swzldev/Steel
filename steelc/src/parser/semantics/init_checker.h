#pragma once

#include <string>
#include <map>
#include <unordered_set>

#include "../ast/ast_visitor.h"
#include "../compilation_pass.h"
#include "../symbolics/symbol_table.h"

class init_checker : public ast_visitor, public compilation_pass {
public:
	init_checker(std::shared_ptr<compilation_unit> unit)
		: compilation_pass(unit) {
	}

	void visit(std::shared_ptr<class type_declaration> decl) override;
	void visit(std::shared_ptr<class constructor_declaration> ctor_decl) override;
	void visit(std::shared_ptr<class function_declaration> func_decl) override;
	void visit(std::shared_ptr<class variable_declaration> var) override;
	void visit(std::shared_ptr<class assignment_expression> expr) override;
	void visit(std::shared_ptr<class identifier_expression> expr) override;
	void visit(std::shared_ptr<class block_statement> block) override;
	void visit(std::shared_ptr<class if_statement> if_stmt) override;
	void visit(std::shared_ptr<class for_loop> for_loop) override;
	void visit(std::shared_ptr<class while_loop> while_loop) override;

private:
	std::unordered_set<std::shared_ptr<variable_declaration>> initialized;
	bool in_method = false;

	void traverse_block(std::shared_ptr<block_statement>& block, bool restore);
};