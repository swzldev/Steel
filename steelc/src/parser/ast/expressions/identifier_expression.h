#pragma once

#include <string>
#include <memory>

#include "expression.h"
#include "../declarations/variable_declaration.h"

class identifier_expression : public expression, public std::enable_shared_from_this<identifier_expression> {
public:
	ENABLE_ACCEPT(identifier_expression)

	identifier_expression(std::string identifier)
		: identifier(identifier) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Identifier \"" + identifier + "\"";
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<identifier_expression>(identifier);
		cloned->declaration = declaration;
		return cloned;
	}

	type_ptr type() const override {
		return declaration ? declaration->type : data_type::UNKNOWN;
	}
	bool is_rvalue() const override {
		return false;
	}

	std::string identifier;
	std::shared_ptr<variable_declaration> declaration;
};