#pragma once

#include <string>
#include <memory>

#include "../ast_fwd.h"

class identifier_expression : public expression, public std::enable_shared_from_this<identifier_expression> {
public:
	ENABLE_ACCEPT(identifier_expression)

	identifier_expression(std::string identifier)
		: identifier(identifier) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Identifier \"" + identifier + "\"";
	}

	type_ptr type() const override {
		return declaration ? declaration->type : data_type::UNKNOWN;
	}
	bool is_rvalue() const override {
		return false;
	}

	std::string identifier;
	ast_ptr<variable_declaration> declaration;
};