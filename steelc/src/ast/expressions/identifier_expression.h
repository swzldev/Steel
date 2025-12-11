#pragma once

#include <string>
#include <memory>

#include <ast/expressions/expression.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity.h>
#include <representations/entities/variable_entity.h>

struct module_info;

class identifier_expression : public expression, public std::enable_shared_from_this<identifier_expression> {
public:
	ENABLE_ACCEPT(identifier_expression)

	identifier_expression(std::string identifier)
		: identifier(identifier), resolved_entity(nullptr) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Identifier \"" + identifier + "\"";
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<identifier_expression>(identifier);
		cloned->resolved_entity = resolved_entity;
		return cloned;
	}

	type_ptr type() const override {
		if (resolved_entity && resolved_entity->kind() == ENTITY_VARIABLE) {
			return resolved_entity->as_variable()->var_type();
		}
		return data_type::UNKNOWN;
	}
	bool is_rvalue() const override {
		// only modifiable if its a variable
		return resolved_entity && resolved_entity->kind() != ENTITY_VARIABLE;
	}
	bool is_constant() const override {
		// TODO: we could allow for CONSTANT variables here
		return false;
	}

	entity_ptr entity() const override {
		return resolved_entity ? resolved_entity : entity::UNRESOLVED;
	}

	std::string identifier;
	entity_ptr resolved_entity;
};