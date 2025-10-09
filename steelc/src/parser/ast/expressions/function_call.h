#pragma once

#include <string>
#include <memory>
#include <iostream>

#include "../ast_fwd.h"
#include "../../parser_utils.h"
#include "../../types/types.h"

class function_call : public expression, public std::enable_shared_from_this<function_call> {
public:
	ENABLE_ACCEPT(function_call)

	function_call(std::string function_name, std::vector<ast_ptr<expression>> args)
		: identifier(function_name), args(args), declaration(nullptr) {
	}
	function_call(ast_ptr<expression> callee, std::string name, std::vector<ast_ptr<expression>> args)
		: identifier(name), callee(callee), args(args), declaration(nullptr) {
	}
	function_call(ast_ptr<expression> callee, std::vector<ast_ptr<expression>> args)
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
		if (override_return_type) {
			return override_return_type;
		}
		if (!declaration) {
			if (ctor_type) {
				return ctor_type->type();
			}
			return data_type::UNKNOWN;
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
	ast_ptr<expression> callee;
	std::vector<ast_ptr<expression>> args;
	data_type_ptr override_return_type = nullptr; // TEMPORARY
	std::vector<ast_ptr<function_declaration>> declaration_candidates;
	ast_ptr<function_declaration> declaration;
	std::vector<type_handle> generic_args;
	ast_ptr<type_declaration> ctor_type = nullptr;
};