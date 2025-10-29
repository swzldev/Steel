#pragma once

#include <string>
#include <memory>

#include "../ast_node.h"
#include "expression.h"
#include "identifier_expression.h"

class member_expression : public expression, public std::enable_shared_from_this<member_expression> {
public:
	ENABLE_ACCEPT(member_expression)

	member_expression(std::shared_ptr<expression> object, std::string member)
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

	ast_ptr clone() const override {
		auto cloned = std::make_shared<member_expression>(
			std::dynamic_pointer_cast<expression>(object->clone()),
			member
		);
		cloned->declaration = declaration;
		return cloned;
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

	std::shared_ptr<expression> object;
	std::string member;
	std::shared_ptr<variable_declaration> declaration;
};