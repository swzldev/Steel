#pragma once

#include <string>
#include <memory>

#include "../ast_node.h"
#include "expression.h"
#include "../../types/types_fwd.h"
#include "../../types/data_type.h"
#include "../../entities/entities_fwd.h"
#include "../../../lexer/token_type.h"

class member_expression : public expression, public std::enable_shared_from_this<member_expression> {
public:
	ENABLE_ACCEPT(member_expression)

	member_expression(std::shared_ptr<expression> object, std::string member, token_type access_operator)
		: object(object), member(member), access_operator(access_operator), resolved_type(nullptr) {
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
			member,
			access_operator
		);
		cloned->resolved_type = resolved_type;
		cloned->resolved_entity = resolved_entity;
		return cloned;
	}

	virtual type_ptr type() const override {
		return resolved_type ? resolved_type : data_type::UNKNOWN;
	}
	bool is_rvalue() const override {
		return !is_lvalue;
	}
	bool is_constant() const override {
		return false;
	}

	entity_ptr entity() const override {
		return resolved_entity;
	}

	inline bool is_resolved() const {
		return resolved_type != nullptr && resolved_entity != nullptr;
	}

	inline bool is_static_access() const {
		return access_operator == TT_SCOPE;
	}
	inline bool is_instance_access() const {
		return access_operator == TT_ACCESS || access_operator == TT_ARROW;
	}

	std::shared_ptr<expression> object;
	std::string member;
	token_type access_operator;
	type_ptr resolved_type;
	entity_ptr resolved_entity;
	bool is_lvalue = true;
};