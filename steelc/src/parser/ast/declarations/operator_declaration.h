#pragma once

#include <string>
#include <memory>

#include "declaration.h"
#include "../../parser_utils.h"
#include "../../types/types.h"
#include "../../../lexer/token_type.h"

class operator_declaration : public declaration, public std::enable_shared_from_this<operator_declaration> {
public:
	ENABLE_ACCEPT(operator_declaration)

	operator_declaration() = default;
	operator_declaration(type_ptr result, type_ptr left, token_type oparator, type_ptr right, ast_ptr body)
		: result_type(result), left(left), right(right), oparator(oparator), body(body) {
	}
	operator_declaration(primitive_type result, primitive_type left, token_type oparator, primitive_type right)
		: result_type(to_data_type(result)), left(to_data_type(left)), right(to_data_type(right)), oparator(oparator), body(nullptr) {
		// used for built in functions that arnt defined in source
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Operator Declaration:\n";
		return result;
	}

	bool matches(const type_ptr& left, token_type oparator, const type_ptr& right) const {
		return *this->left == *left && this->oparator == oparator && *this->right == *right;
	}

	type_ptr result_type;
	type_ptr left;
	type_ptr right;
	token_type oparator;
	ast_ptr body;
};