#pragma once

#include <string>
#include <memory>

#include <ast/expressions/expression.h>
#include <representations/types/types_fwd.h>

class this_expression : public expression, public std::enable_shared_from_this<this_expression> {
public:
	ENABLE_ACCEPT(this_expression)

	this_expression() {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "This Expression";
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<this_expression>();
		cloned->span = span;
		cloned->parent_type = parent_type;
		return cloned;
	}

	type_ptr type() const override {
		return parent_type;
	}
	bool is_rvalue() const override {
		return false;
	}
	bool is_constant() const override {
		return false;
	}

	type_ptr parent_type;
};