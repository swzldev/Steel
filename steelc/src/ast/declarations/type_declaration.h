#pragma once

#include <string>
#include <memory>
#include <vector>

#include <ast/declarations/declaration.h>
#include <ast/declarations/variable_declaration.h>
#include <ast/declarations/function_declaration.h>
#include <ast/declarations/operator_declaration.h>
#include <representations/types/types_fwd.h>
#include <representations/types/custom_type.h>
#include <representations/entities/type_entity.h>

class type_declaration : public declaration, public std::enable_shared_from_this<type_declaration> {
public:
	ENABLE_ACCEPT(type_declaration)

	type_declaration(custom_type_type kind, const std::string& identifier)
		: identifier(identifier), type_kind(kind) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Type Declaration:\n";
		result += ind + " Identifier: " + identifier + "\n";
		result += ind + " Type Kind: " + (type_kind == CT_STRUCT ? "Struct" : type_kind == CT_CLASS ? "Class" : type_kind == CT_INTERFACE ? "Interface" : "Unknown") + "\n";
		result += ind + " Constructors:\n";
		if (constructors.empty()) {
			result += ind + "  <None>\n";
		}
		else {
			for (const auto& constructor : constructors) {
				result += constructor->string(indent + 1) + "\n";
			}
		}
		result += ind + " Members:\n";
		if (fields.empty()) {
			result += ind + "  <None>\n";
		}
		else {
			for (const auto& member : fields) {
				result += member->string(indent + 1) + "\n";
			}
		}
		result += ind + " Methods:\n";
		if (methods.empty()) {
			result += ind + "  <None>\n";
		}
		else {
			for (const auto& method : methods) {
				result += method->string(indent + 1) + "\n";
			}
		}
		return result;
	}

	type_ptr type() {
		std::shared_ptr<custom_type> type = nullptr;
		if (is_generic && is_generic_instance) {
			std::vector<type_ptr> generic_args;
			for (const auto& gen : generics) {
				if (!gen->substitution || gen->substitution == data_type::UNKNOWN) {
					return nullptr;
				}
				generic_args.push_back(gen->substitution);
			}
			type = custom_type::get(identifier, generic_args);
		}
		else {
			type = custom_type::get(identifier);
		}
		if (!type->declaration) {
			type->declaration = shared_from_this();
		}
		return type;
	}
	std::string name() const {
		return identifier;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<type_declaration>(type_kind, identifier);
		cloned->span = span;
		cloned->owning_unit = owning_unit;
		cloned->parent_module = parent_module;
		cloned->base_types = base_types;
		cloned->base_type = base_type;
		for (const auto& gen : generics) {
			cloned->generics.push_back(std::dynamic_pointer_cast<generic_parameter>(gen->clone()));
		}
		for (const auto& constructor : constructors) {
			cloned->constructors.push_back(std::dynamic_pointer_cast<function_declaration>(constructor->clone()));
		}
		for (const auto& field : fields) {
			cloned->fields.push_back(std::dynamic_pointer_cast<variable_declaration>(field->clone()));
		}
		for (const auto& method : methods) {
			cloned->methods.push_back(std::dynamic_pointer_cast<function_declaration>(method->clone()));
		}
		for (const auto& op : operators) {
			cloned->operators.push_back(std::dynamic_pointer_cast<operator_declaration>(op->clone()));
		}
		cloned->is_generic = is_generic;
		cloned->is_generic_instance = is_generic_instance;
		// do not clone cached entity
		return cloned;
	}

	custom_type_type type_kind;
	std::string identifier;
	std::vector<type_ptr> base_types;
	std::shared_ptr<type_declaration> base_type;
	std::vector<std::shared_ptr<generic_parameter>> generics;
	std::vector<std::shared_ptr<function_declaration>> constructors;
	std::vector<std::shared_ptr<variable_declaration>> fields;
	std::vector<std::shared_ptr<function_declaration>> methods;
	std::vector<std::shared_ptr<operator_declaration>> operators;
	bool is_generic = false;
	bool is_generic_instance = false;
};