#include "symbol_table.h"

#include <memory>
#include <string>
#include <vector>

#include <symbolics/lookup_result.h>
#include <symbolics/symbol_error.h>
#include <ast/generics/generic_parameter.h>
#include <ast/declarations/type_declaration.h>
#include <ast/declarations/enum_declaration.h>
#include <ast/declarations/module_declaration.h>
#include <representations/entities/entity.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/variable_entity.h>
#include <representations/entities/function_entity.h>
#include <representations/entities/type_entity.h>
#include <representations/entities/module_entity.h>
#include <representations/entities/generic_param_entity.h>

symbol_table::symbol_table() {
	variable_scopes = {};
	variable_scopes.emplace_back(); // global scope
	// generic scopes dont need a global scope
	// since they can only exist in a declaration
}

void symbol_table::push_scope() {
	variable_scopes.emplace_back();
}
void symbol_table::pop_scope() {
	if (!variable_scopes.empty()) {
		variable_scopes.pop_back();
	}
}
void symbol_table::push_generic_scope()
{
	generic_scopes.emplace_back();
}
void symbol_table::pop_generic_scope()
{
	if (!generic_scopes.empty()) {
		generic_scopes.pop_back();
	}
}

add_symbol_result symbol_table::add_symbol(entity_ptr entity, std::shared_ptr<type_entity> owner) {
	symbol_error err = SYMBOL_OK;
	switch (entity->kind()) {
	case ENTITY_VARIABLE: {
		if (owner) {
			err = owner->symbols.add_field(entity->name(), entity->as_variable());
		}
		else err = add_variable(entity->as_variable());
		break;
	}
	case ENTITY_FUNCTION: {
		if (owner) {
			err = owner->symbols.add_method(entity->name(), entity->as_function());
		}
		else err = add_function(entity->as_function());
		break;
	}
	case ENTITY_TYPE: {
		// TODO: add support for nested types later
		err = add_type(entity->as_type());
		break;
	}
	case ENTITY_MODULE: {
		err = add_module(entity->as_module());
		break;
	}
	case ENTITY_GENERIC_PARAM: {
		err = add_generic(entity->as_generic_param());
		break;
	}
	}
	if (err == SYMBOL_OK) {
		// add to entity list if successfully added
		register_entity(entity, next_entity_id++);
	}
	return add_symbol_result(err);
}
add_symbol_result symbol_table::add_symbol(std::shared_ptr<variable_declaration> var, std::shared_ptr<type_entity> owner) {
	return add_symbol(variable_entity::get(var), owner);
}
add_symbol_result symbol_table::add_symbol(std::shared_ptr<function_declaration> func, std::shared_ptr<type_entity> owner) {
	return add_symbol(function_entity::get(func), owner);
}
add_symbol_result symbol_table::add_symbol(std::shared_ptr<type_declaration> type) {
	return add_symbol(type_entity::get(type));
}
add_symbol_result symbol_table::add_symbol(std::shared_ptr<enum_declaration> type_enum) {
	return add_symbol(type_entity::get(type_enum));
}
add_symbol_result symbol_table::add_symbol(std::shared_ptr<generic_parameter> param) {
	return add_symbol(generic_param_entity::get(param));
}
add_symbol_result symbol_table::add_symbol(std::shared_ptr<module_declaration> module) {
	return add_symbol(module->entity);
}

lookup_result symbol_table::lookup(const std::string& name) const {
	std::vector<lookup_result> results;

	results.push_back(get_variable(name));
	results.push_back(get_generic(name));
	results.push_back(get_functions(name));
	results.push_back(get_type(name));
	results.push_back(get_module(name));

	lookup_result final_result;
	for (const auto& res : results) {
		if (!res.results.empty()) {
			for (const auto& ent : res.results) {
				final_result.results.push_back(ent);
			}
		}
	}
	return final_result;
}

entity_ptr symbol_table::get_by_id(entity_id id) const {
	if (id_to_entities.contains(id)) {
		return id_to_entities.at(id);
	}
	return nullptr;
}

entity_id symbol_table::next_entity_id = 0;

void symbol_table::register_entity(entity_ptr entity, entity_id id) {
	id_to_entities[id] = entity;
	entity->id = id; // allowed since symbol_table is a friend class
}

lookup_result symbol_table::get_variable(const std::string& name) const {
	// search upwards from current scope
	for (auto it = variable_scopes.rbegin(); it != variable_scopes.rend(); ++it) {
		auto found = it->find(name);
		if (found != it->end()) {
			return { found->second };
		}
	}
	return {};
}
lookup_result symbol_table::get_functions(const std::string& name) const {
	lookup_result result;
	// local lookup (ignore parameters, etc)
	auto it = functions.find(name);
	if (it != functions.end()) {
		for (const auto& func : it->second) {
			result.results.push_back(func);
		}
	}
	return result;
}
lookup_result symbol_table::get_type(const std::string& name) const {
	// local lookup
	auto it = types.find(name);
	if (it != types.end()) {
		return { it->second };
	}
	return {};
}
lookup_result symbol_table::get_module(const std::string& name) const {
	// local lookup
	auto it = modules.find(name);
	if (it != modules.end()) {
		return { it->second };
	}
	return {};
}
lookup_result symbol_table::get_generic(const std::string& name) const {
	// local lookup
	for (auto it = generic_scopes.rbegin(); it != generic_scopes.rend(); ++it) {
		auto found = it->find(name);
		if (found != it->end()) {
			return { found->second };
		}
	}
	return {};
}

symbol_error symbol_table::add_variable(std::shared_ptr<variable_entity> var) {
	auto& current_scope = variable_scopes.back();
	// variable_scopes cannot shadow generics
	for (const auto& gen_scope : generic_scopes) {
		if (gen_scope.find(var->name()) != gen_scope.end()) {
			return SYMBOL_CONFLICTS_WITH_GENERIC;
		}
	}
	if (current_scope.find(var->name()) != current_scope.end()) {
		return SYMBOL_CONFLICTS_WITH_VARIABLE;
	}
	// module-level variables cannot conflict
	if (in_global_scope()) {
		if (symbol_error err = name_conflicts(var->name()); err != SYMBOL_OK) {
			return err;
		}
	}
	current_scope[var->name()] = var;
	return SYMBOL_OK;
}
symbol_error symbol_table::add_generic(std::shared_ptr<generic_param_entity> param) {
	auto& current_scope = generic_scopes.back();
	// generics cannot shadow variable_scopes
	for (const auto& var_scope : variable_scopes) {
		if (var_scope.find(param->name()) != var_scope.end()) {
			return SYMBOL_CONFLICTS_WITH_VARIABLE;
		}
	}
	if (current_scope.find(param->name()) != current_scope.end()) {
		return SYMBOL_CONFLICTS_WITH_GENERIC;
	}
	current_scope[param->name()] = param;
	return SYMBOL_OK;
}
symbol_error symbol_table::add_function(std::shared_ptr<function_entity> func) {
	for (const auto& existing : functions) {
		for (const auto& existing_overload : existing.second) {
			if (existing_overload->name() == func->name()) {
				const auto& existing_overload_type = existing_overload->as_function()->function_type();
				const auto& func_type = func->as_function()->function_type();

				// check return type matches
				if (*existing_overload_type->get_return_type() != func_type->get_return_type()) {
					return SYMBOL_CANNOT_OVERLOAD_BY_RETURN_TYPE;
				}

				// check params match
				if (existing_overload_type->params_match(func_type->get_parameter_types())) {
					return SYMBOL_CONFLICTS_WITH_FUNCTION;
				}
			}
		}
	}
	// we can allow conflicts with functions here (for overloading)
	symbol_error err = name_conflicts(func->name());
	if (err != SYMBOL_OK && err != SYMBOL_CONFLICTS_WITH_FUNCTION) {
		return err;
	}
	functions[func->name()].push_back(func);
	return SYMBOL_OK;
}
symbol_error symbol_table::add_type(std::shared_ptr<type_entity> type) {
	if (symbol_error err = name_conflicts(type->name()); err != SYMBOL_OK) {
		return err;
	}
	types[type->name()] = type;
	return SYMBOL_OK;
}
symbol_error symbol_table::add_module(std::shared_ptr<module_entity> module) {
	if (symbol_error err = name_conflicts(module->name()); err != SYMBOL_OK) {
		return err;
	}
	modules[module->name()] = module;
	return SYMBOL_OK;
}

symbol_error symbol_table::name_conflicts(const std::string& name) {
	// check if conflicts with unscoped-variable
	const auto& global_var_scope = variable_scopes[0];
	if (global_var_scope.find(name) != global_var_scope.end()) {
		return SYMBOL_CONFLICTS_WITH_VARIABLE;
	}
	// check if conflicts with function
	if (functions.find(name) != functions.end()) {
		return SYMBOL_CONFLICTS_WITH_FUNCTION;
	}
	// check if conflicts with type
	if (auto type_it = types.find(name); type_it != types.end()) {
		if (type_it->second->type->is_enum()) {
			return SYMBOL_CONFLICTS_WITH_ENUM;
		}
		return SYMBOL_CONFLICTS_WITH_TYPE;
	}
	// check if conflicts with module
	if (modules.find(name) != modules.end()) {
		return SYMBOL_CONFLICTS_WITH_MODULE;
	}
	return SYMBOL_OK;
}
