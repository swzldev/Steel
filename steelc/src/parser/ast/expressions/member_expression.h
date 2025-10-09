#pragma once

#include <string>
#include <memory>

#include "../ast_fwd.h"

class member_expression : public expression, public std::enable_shared_from_this<member_expression> {
public:
	ENABLE_ACCEPT(member_expression)

	member_expression(ast_ptr<expression> object, std::string member)
		: object(object), member(member) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Member Expression:\n";
		result += ind + " Left Identifier:\n";
		if (object) {
			result += object->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		result += ind + " Member Identifier:\n";
		result += ind + "  \"" + member + "\"\n";
		return result;
	}

	virtual type_ptr type() const override {
		if (!declaration || !declaration->type) {
			return data_type::UNKNOWN;
		}
		return declaration->type;
	}
	bool is_rvalue() const override {
		return false;
	}

	ast_ptr<expression> object;
	std::string member;
	ast_ptr<variable_declaration> declaration;
};