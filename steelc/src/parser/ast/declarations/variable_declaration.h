#pragma once

#include <string>

#include "declaration.h"
#include "../expressions/expression.h"
#include "../../types/data_type.h"

class variable_declaration : public declaration, public std::enable_shared_from_this<variable_declaration> {
public:
	ENABLE_ACCEPT(variable_declaration)

	variable_declaration(bool is_const, type_ptr type, std::string identifier)
		: is_const(is_const), type(type), identifier(identifier), initializer(nullptr) {
	}
	variable_declaration(bool is_const, type_ptr type, std::string identifier, std::shared_ptr<expression> initializer)
		: is_const(is_const), type(type), identifier(identifier), initializer(initializer), initialized(initializer) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Variable Declaration:\n";
		result += ind + " Type: " + type->name() + "\n";
		result += ind + " Identifier: " + identifier + "\n";
		result += ind + " Initializer:\n";
		if (initializer) {
			result += initializer->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		return result;
	}

	inline bool has_initializer() const {
		return initializer != nullptr;
	}

	type_ptr type;
	std::string identifier;
	std::shared_ptr<expression> initializer;
	bool initialized = false;
	bool is_const = false;
	bool is_parameter = false;
};