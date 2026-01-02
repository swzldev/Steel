#pragma once

#include <string>
#include <memory>
#include <iostream>

#include <ast/expressions/expression.h>
#include <ast/declarations/function_declaration.h>
#include <ast/declarations/type_declaration.h>
#include <representations/types/types_fwd.h>

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

	ast_ptr clone() const override {
		auto cloned = std::make_shared<function_call>(
			identifier,
			std::vector<std::shared_ptr<expression>>{}
		);
		cloned->span = span;
		if (callee) {
			cloned->callee = std::dynamic_pointer_cast<expression>(callee->clone());
		}
		for (const auto& arg : args) {
			cloned->args.push_back(std::dynamic_pointer_cast<expression>(arg->clone()));
		}
		cloned->declaration = declaration;
		cloned->generic_args = generic_args;
		cloned->ctor_type = ctor_type;
		return cloned;
	}

	type_ptr type() const override {
		// returns the RETURN TYPE not the FUNCTION TYPE
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
	bool is_constant() const override {
		return false;
	}

	inline bool is_scoped_function() const {
		return scope != nullptr;
		// e.g. module::function()
		// sort of half way between a function and method
	}
	inline bool is_method() const {
		return callee != nullptr;
	}

	std::string identifier;
	std::shared_ptr<expression> scope;
	std::shared_ptr<expression> callee;
	std::vector<std::shared_ptr<expression>> args;
	std::vector<std::shared_ptr<function_declaration>> declaration_candidates;
	std::shared_ptr<function_declaration> declaration;
	std::vector<type_ptr> generic_args;
	std::shared_ptr<type_declaration> ctor_type = nullptr;
	bool is_constructor = false;
};