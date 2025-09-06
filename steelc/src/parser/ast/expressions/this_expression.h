#pragma once

#include <string>
#include <memory>

#include "expression.h"
#include "../declarations/variable_declaration.h"

class this_expression : public expression, public std::enable_shared_from_this<this_expression> {
public:
	ENABLE_ACCEPT(this_expression)

	this_expression() {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "This Expression";
	}

	type_ptr type() const override {
		return parent_type;
	}
	bool is_rvalue() const override {
		return false;
	}

	type_ptr parent_type;
};