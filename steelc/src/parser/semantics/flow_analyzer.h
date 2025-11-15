#pragma once

#include <string>
#include <map>
#include <unordered_set>

#include "../ast/ast_visitor.h"
#include "../compilation_pass.h"
#include "../symbolics/symbol_table.h"

class flow_analyzer : public ast_visitor, public compilation_pass {
public:
	flow_analyzer(std::shared_ptr<compilation_unit> unit)
		: current_function(nullptr), current_constructor(nullptr), in_loop(false), current_returns(false), current_conditionally_returns(false), compilation_pass(unit) {
	}

	void visit(std::shared_ptr<function_declaration> func) override;
	void visit(std::shared_ptr<code_block> block) override;
	void visit(std::shared_ptr<return_statement> ret_stmt) override;
	void visit(std::shared_ptr<break_statement> brk_stmt) override;
	void visit(std::shared_ptr<if_statement> if_stmt) override;
	void visit(std::shared_ptr<for_loop> for_loop) override;
	void visit(std::shared_ptr<while_loop> while_loop) override;

private:
	std::shared_ptr<function_declaration> current_function;
	std::shared_ptr<function_declaration> current_constructor;
	bool in_loop;

	bool current_conditionally_returns;
	bool current_returns;
};