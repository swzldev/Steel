#pragma once

#include <string>

#include <ast/declarations/declaration.h>
#include <ast/expressions/expression.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/entities/entity_ref.h>

class variable_declaration : public declaration, public std::enable_shared_from_this<variable_declaration> {
public:
	ENABLE_ACCEPT(variable_declaration)

	variable_declaration(bool is_const, type_ptr type, std::string identifier)
		: is_const(is_const), type(type), identifier(identifier), initializer(nullptr) {
	}
	variable_declaration(bool is_const, type_ptr type, std::string identifier, std::shared_ptr<expression> initializer)
		: is_const(is_const), type(type), identifier(identifier), initializer(initializer), initialized(initializer) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Variable Declaration:\n";
		result += ind + " Type: " + type->name() + "\n";
		result += ind + " Identifier: " + identifier + "\n";
		result += ind + " Initializer:\n";
		if (initializer) {
			result += initializer->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		return result;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<variable_declaration>(
			is_const,
			type,
			identifier
		);
		cloned->owning_unit = owning_unit;
		cloned->parent_module = parent_module;
		cloned->span = span;
		if (initializer) {
			cloned->initializer = std::dynamic_pointer_cast<expression>(initializer->clone());
		}
		cloned->initialized = initialized;
		cloned->is_parameter = is_parameter;
		cloned->is_field = is_field;
		cloned->field_index = field_index;
		return cloned;
	}

	inline bool has_initializer() const {
		return initializer != nullptr;
	}

	type_ptr type;
	std::string identifier;
	std::shared_ptr<expression> initializer;
	bool initialized = false;
	bool is_const = false;
	bool is_parameter = false;
	bool is_field = false;
	size_t field_index = 0;
	entity_ref var_ref;
};