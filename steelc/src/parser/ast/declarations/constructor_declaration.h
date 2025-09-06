#pragma once

#include <string>
#include <vector>

#include "declaration.h"
#include "variable_declaration.h"
#include "../../parser_utils.h"
#include "../../types/types.h"

class constructor_declaration : public declaration, public std::enable_shared_from_this<constructor_declaration> {
public:
	ENABLE_ACCEPT(constructor_declaration)

	constructor_declaration(std::vector<std::shared_ptr<variable_declaration>> parameters, ast_ptr body)
		: parameters(parameters), body(body) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Constructor Declaration:\n";
		result += ind + " Parameters:\n";
		if (parameters.size() <= 0) {
			result += ind + "  <None>\n";
		}
		else {
			for (const auto& param : parameters) {
				result += ind + "  - " + param->type->type_name() + " " + param->identifier + "\n";
			}
		}
		if (body) {
			result += ind + " Body:\n";
			result += body->string(indent + 1);
		}
		else {
			result += ind + " Body: Not defined\n";
		}
		return result;
	}

	inline std::vector<type_ptr> get_expected_types() {
		std::vector<type_ptr> expected_types;
		for (const auto& param : parameters) {
			expected_types.push_back(param->type);
		}
		return expected_types;
	}

	std::vector<std::shared_ptr<variable_declaration>> parameters;
	ast_ptr body;
};