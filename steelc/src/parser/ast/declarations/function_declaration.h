#pragma once

#include <string>
#include <vector>

#include "declaration.h"
#include "variable_declaration.h"
#include "../../parser_utils.h"
#include "../../types/data_type.h"

class function_declaration : public declaration, public std::enable_shared_from_this<function_declaration> {
public:
	ENABLE_ACCEPT(function_declaration)

	function_declaration() = default;
	function_declaration(type_ptr return_type, std::string identifier, std::vector<std::shared_ptr<variable_declaration>> parameters)
		: return_type(return_type), identifier(identifier), parameters(parameters), body(nullptr) /* for built in functions */ {
	}
	function_declaration(type_ptr return_type, std::string identifier, std::vector<std::shared_ptr<variable_declaration>> parameters, ast_ptr body, bool is_override)
		: return_type(return_type), identifier(identifier), parameters(parameters), body(body), is_override(is_override) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Function Declaration: \"" + identifier + "\"\n";
		result += ind + " Type: " + return_type->name() + "\n";
		result += ind + " Parameters:\n";
		if (parameters.empty()) {
			result += ind + "  <None>\n";
		}
		else {
			for (int i = 0; i < parameters.size(); i++) {
				const auto& param = parameters[i];
				result += ind + "  - " + param->type->name() + " " + param->identifier + "\n";
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

	inline bool is_abstract() const {
		return body == nullptr;
	}
	inline bool matches(const std::shared_ptr<function_declaration>& other, bool match_return_type = true) {
		if (!other) {
			return false;
		}
		if (identifier != other->identifier) {
			return false;
		}
		if (parameters.size() != other->parameters.size()) {
			return false;
		}
		for (size_t i = 0; i < parameters.size(); i++) {
			if (*parameters[i]->type != other->parameters[i]->type) {
				return false;
			}
		}
		if (match_return_type && *return_type != other->return_type) {
			return false;
		}
		return true;
	}
	inline std::vector<type_ptr> get_expected_types() {
		std::vector<type_ptr> expected_types;
		for (const auto& param : parameters) {
			expected_types.push_back(param->type);
		}
		return expected_types;
	}

	type_ptr return_type;
	std::string identifier;
	std::vector<std::shared_ptr<generic_parameter>> generics;
	std::vector<type_ptr> generic_substitutions;
	std::vector<std::shared_ptr<variable_declaration>> parameters;
	ast_ptr body;
	bool is_method = false;
	bool is_generic = false;
	bool is_override = false;
	bool is_constructor = false;
	bool is_constrained = false;
	bool is_generic_instance = false;
	std::shared_ptr<function_declaration> overridden_function = nullptr;
};