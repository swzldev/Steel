#pragma once

#include <string>
#include <memory>

#include "expression.h"
#include "../declarations/variable_declaration.h"
#include "../declarations/function_declaration.h"
#include "../declarations/type_declaration.h"
#include "../declarations/enum_declaration.h"

enum identifier_type {
	IDENTIFIER_UNKNOWN,
	IDENTIFIER_VARIABLE,
	IDENTIFIER_FUNCTION,
	IDENTIFIER_TYPE,
	IDENTIFIER_ENUM,
};

class identifier_expression : public expression, public std::enable_shared_from_this<identifier_expression> {
public:
	ENABLE_ACCEPT(identifier_expression)

	identifier_expression(std::string identifier)
		: identifier(identifier), id_type(IDENTIFIER_UNKNOWN) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Identifier \"" + identifier + "\"";
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<identifier_expression>(identifier);
		cloned->id_type = id_type;
		cloned->variable_declaration = variable_declaration;
		cloned->function_declaration = function_declaration;
		cloned->type_declaration = type_declaration;
		cloned->enum_declaration = enum_declaration;
		return cloned;
	}

	type_ptr type() const override {
		if (variable_declaration && id_type == IDENTIFIER_VARIABLE) {
			return variable_declaration->type;
		}
		else if (function_declaration && id_type == IDENTIFIER_FUNCTION) {
			return data_type::UNKNOWN; // TODO
		}
		else if (type_declaration && id_type == IDENTIFIER_TYPE) {
			return type_declaration->type();
		}
		else if (enum_declaration && id_type == IDENTIFIER_ENUM) {
			return enum_declaration->type();
		}
		return data_type::UNKNOWN;
	}
	bool is_rvalue() const override {
		// only modifiable if its a variable
		return id_type != IDENTIFIER_VARIABLE;
	}

	std::string identifier;
	identifier_type id_type;
	std::shared_ptr<variable_declaration> variable_declaration;
	std::shared_ptr<function_declaration> function_declaration;
	std::shared_ptr<type_declaration> type_declaration;
	std::shared_ptr<enum_declaration> enum_declaration;
};