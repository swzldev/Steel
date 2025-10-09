#pragma once

#include <string>
#include <memory>

#include "../ast_node.h"
#include "../ast_fwd.h"

class expression_statement : public ast_node, public std::enable_shared_from_this<expression_statement> {
public:
	ENABLE_ACCEPT(expression_statement)

	expression_statement(ast_ptr<expression> expr)
		: expr(expr) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Expression Statement:\n" + expr->string(indent + 1);
	}

	ast_ptr<expression> expr;
};