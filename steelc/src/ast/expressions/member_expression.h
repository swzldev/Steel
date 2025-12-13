#pragma once

#include <string>
#include <memory>

#include <ast/ast_node.h>
#include <ast/expressions/expression.h>
#include <lexer/token_type.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity.h>
#include <representations/entities/variable_entity.h>

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
		cloned->position = position;
		cloned->resolved_type = resolved_type;
		cloned->resolved_entity = resolved_entity;
		return cloned;
	}

	virtual type_ptr type() const override {
		// this should be deprecated soon, should fully switch to using the entity
		if (!resolved_type && resolved_entity && resolved_entity->kind() == ENTITY_VARIABLE) {
			return resolved_entity->as_variable()->var_type();
		}
		return resolved_type ? resolved_type : data_type::UNKNOWN;
	}
	bool is_rvalue() const override {
		return !is_lvalue;
	}
	bool is_constant() const override {
		return false;
	}

	entity_ptr entity() const override {
		return resolved_entity ? resolved_entity : entity::UNRESOLVED;
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