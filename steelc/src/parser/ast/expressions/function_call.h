#pragma once

#include <string>
#include <memory>
#include <iostream>

#include "expression.h"
#include "../declarations/function_declaration.h"
#include "../../parser_utils.h"
#include "../../types/types.h"

class function_call : public expression, public std::enable_shared_from_this<function_call> {
public:
	ENABLE_ACCEPT(function_call)

	function_call(std::string function_name, std::vector<std::shared_ptr<expression>> args)
		: identifier(function_name), args(args), declaration(nullptr) {
	}
	function_call(std::shared_ptr<expression> callee, std::string name, std::vector<std::shared_ptr<expression>> args)
		: identifier(name), callee(callee), args(args), declaration(nullptr) {
	}
	function_call(std::shared_ptr<expression> callee, std::vector<std::shared_ptr<expression>> args)
		: identifier(""), callee(callee), args(args), declaration(nullptr) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Function Call: \"" + identifier + "\"\n";
		result += ind + " Arguments:\n";
		if (args.size() <= 0) {
			result += ind + "  <None>\n";
		}
		else {
			for (int i = 0; i < args.size(); i++) {
				result += args[i]->string(indent + 1) + "\n";
			}
		}
		return result;
	}

	type_ptr type() const override {
		if (!declaration) {
			return data_type::unknown;
		}
		return declaration->return_type;
	}
	bool is_rvalue() const override {
		return true; // function calls always return a temporary value
	}

	inline bool is_method() const {
		return callee != nullptr;
	}
	inline bool is_constructor() const {
		return ctor_type != nullptr;
	}

	std::string identifier;
	std::shared_ptr<expression> callee;
	std::vector<std::shared_ptr<expression>> args;
	std::vector<std::shared_ptr<function_declaration>> declaration_candidates;
	std::shared_ptr<function_declaration> declaration;
	std::vector<type_ptr> generic_args;
	std::shared_ptr<type_declaration> ctor_type = nullptr;
};