#pragma once

#include <string>
#include <memory>

#include <ast/expressions/expression.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity.h>
#include <representations/entities/entity_ref.h>
#include <representations/entities/variable_entity.h>
#include <symbolics/symbol_table.h>

struct module_info;

class identifier_expression : public expression, public std::enable_shared_from_this<identifier_expression> {
public:
	ENABLE_ACCEPT(identifier_expression)

	identifier_expression(std::string identifier)
		: identifier(identifier) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Identifier \"" + identifier + "\"";
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<identifier_expression>(identifier);
		cloned->position = position;
		cloned->entity_ref = entity_ref;
		return cloned;
	}

	type_ptr type() const override {
		return data_type::UNKNOWN;
	}
	bool is_rvalue() const override {
		// technically yes if variable, but we dont have that
		// kind of context here, maybe we will change this later
		return false;
	}
	bool is_constant() const override {
		// TODO: we could allow for CONSTANT variables here
		return false;
	}

	entity_ptr entity(const symbol_table& sym_table) const override {
		return entity_ref.resolve(sym_table);
	}

	std::string identifier;
	entity_ref entity_ref;
};