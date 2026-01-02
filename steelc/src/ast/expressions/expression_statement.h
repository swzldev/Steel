#pragma once

#include <string>
#include <memory>

#include <ast/ast_node.h>
#include <ast/expressions/expression.h>

class expression_statement : public ast_node, public std::enable_shared_from_this<expression_statement> {
public:
	ENABLE_ACCEPT(expression_statement)

	expression_statement(std::shared_ptr<expression> expr)
		: expr(expr) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Expression Statement:\n" + expr->string(indent + 1);
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<expression_statement>(
			std::dynamic_pointer_cast<expression>(expr->clone())
		);
		cloned->span = span;
		return cloned;
	}

	std::shared_ptr<expression> expr;
};