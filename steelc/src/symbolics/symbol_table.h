#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <memory>

#include <symbolics/lookup_result.h>
#include <symbolics/symbol_error.h>
#include <ast/declarations/variable_declaration.h>
#include <ast/declarations/function_declaration.h>
#include <representations/entities/entities_fwd.h>

struct module_info;

struct add_symbol_result {
	// defaults entityid to -1 (invalid)
	add_symbol_result(symbol_error err)
		: error(err), entityid(-1) {
	}
	// defaults error to SYMBOL_OK
	add_symbol_result(entity_id id)
		: error(SYMBOL_OK), entityid(id) {
	}

	symbol_error error;
	entity_id entityid = -1;
};

class symbol_table {
public:
	symbol_table();

	inline bool in_global_scope() const {
		return variable_scopes.size() <= 1;
	}
	void push_scope();
	void pop_scope();
	void push_generic_scope();
	void pop_generic_scope();

	add_symbol_result add_symbol(entity_ptr entity, std::shared_ptr<type_entity> owner = nullptr);
	
	// helper functions (automatically converts to entity)
	
	// errors:
	// - SYMBOL_CONFLICTS_WITH_VARIABLE
	//    special meaning: another variable with the same name exists in the current scope
	// - SYMBOL_CONFLICTS_WITH_GENERIC
	//    special meaning: variable shadows a generic parameter in the current or any parent scope
	// 
	// (module-level vars only)
	// - SYMBOL_CONFLICTS_WITH_FUNCTION
	// - SYMBOL_CONFLICTS_WITH_TYPE
	// - SYMBOL_CONFLICTS_WITH_ENUM
	// - SYMBOL_CONFLICTS_WITH_MODULE
	add_symbol_result add_symbol(std::shared_ptr<variable_declaration> var, std::shared_ptr<type_entity> owner = nullptr);
	// errors:
	// - SYMBOL_CANNOT_OVERLOAD_BY_RETURN_TYPE
	//    meaning: function with same name and parameter types but different return type exists
	// - SYMBOL_CONFLICTS_WITH_FUNCTION
	//    special meaning: exact same function signature already exists
	// 
	// - SYMBOL_CONFLICTS_WITH_VARIABLE
	//    special meaning: another variable/field with the same name exists in the current scope
	// - SYMBOL_CONFLICTS_WITH_TYPE
	// - SYMBOL_CONFLICTS_WITH_ENUM
	// - SYMBOL_CONFLICTS_WITH_MODULE
	add_symbol_result add_symbol(std::shared_ptr<function_declaration> func, std::shared_ptr<type_entity> owner = nullptr);
	// errors:
	// - SYMBOL_CONFLICTS_WITH_VARIABLE
	// - SYMBOL_CONFLICTS_WITH_FUNCTION
	// - SYMBOL_CONFLICTS_WITH_TYPE
	// - SYMBOL_CONFLICTS_WITH_ENUM
	// - SYMBOL_CONFLICTS_WITH_MODULE
	add_symbol_result add_symbol(std::shared_ptr<type_declaration> type);
	// errors:
	// - SYMBOL_CONFLICTS_WITH_VARIABLE
	// - SYMBOL_CONFLICTS_WITH_FUNCTION
	// - SYMBOL_CONFLICTS_WITH_TYPE
	// - SYMBOL_CONFLICTS_WITH_ENUM
	// - SYMBOL_CONFLICTS_WITH_MODULE
	add_symbol_result add_symbol(std::shared_ptr<enum_declaration> enm);
	// errors:
	// - SYMBOL_CONFLICTS_WITH_VARIABLE
	//    special meaning: generic parameter shadows a variable in the current or any parent scope
	// - SYMBOL_CONFLICTS_WITH_GENERIC
	//    special meaning: duplicate generic parameter in generic declaration
	// 
	// since generics are always scoped, no module-level conflict errors can occur
	add_symbol_result add_symbol(std::shared_ptr<generic_parameter> param);
	// errors:
	// - SYMBOL_CONFLICTS_WITH_VARIABLE
	// - SYMBOL_CONFLICTS_WITH_FUNCTION
	// - SYMBOL_CONFLICTS_WITH_TYPE
	// - SYMBOL_CONFLICTS_WITH_ENUM
	// - SYMBOL_CONFLICTS_WITH_MODULE
	add_symbol_result add_symbol(std::shared_ptr<module_declaration> module);

	inline bool has_variable(const std::string& name) const {
		return get_variable(name).found();
	}
	inline bool has_function(const std::string& name) const {
		return get_functions(name).found();
	}
	inline bool has_type(const std::string& name) const {
		return types.find(name) != types.end();
	}
	inline bool has_type(const std::string& name) const {
		return enums.find(name) != enums.end();
	}
	inline bool has_generic(const std::string& name) const {
		return get_generic(name).found();
	}
	inline bool has_module(const std::string& name) const {
		return modules.find(name) != modules.end();
	}

	lookup_result lookup(const std::string& name) const;

	virtual entity_ptr get_by_id(entity_id id) const;

private:
	static entity_id next_entity_id;
	std::unordered_map<entity_id, entity_ptr> id_to_entities;

	std::vector<std::map<std::string, std::shared_ptr<variable_entity>>> variable_scopes;
	std::vector<std::map<std::string, std::shared_ptr<generic_param_entity>>> generic_scopes;
	std::unordered_map<std::string, std::vector<std::shared_ptr<function_entity>>> functions;
	std::unordered_map<std::string, std::shared_ptr<type_entity>> types;
	std::unordered_map<std::string, std::shared_ptr<enum_entity>> enums;
	std::unordered_map<std::string, std::shared_ptr<module_entity>> modules;

	void register_entity(entity_ptr entity, entity_id id);

	lookup_result get_variable(const std::string& name) const;
	lookup_result get_functions(const std::string& name) const;
	lookup_result get_type(const std::string& name) const;
	lookup_result get_enum(const std::string& name) const;
	lookup_result get_module(const std::string& name) const;
	lookup_result get_generic(const std::string& name) const;

	symbol_error add_variable(std::shared_ptr<variable_entity> var);
	symbol_error add_generic(std::shared_ptr<generic_param_entity> param);
	symbol_error add_function(std::shared_ptr<function_entity> func);
	symbol_error add_type(std::shared_ptr<type_entity> type);
	symbol_error add_enum(std::shared_ptr<enum_entity> enm);
	symbol_error add_module(std::shared_ptr<module_entity> module);

	// detects unscoped variable, function, type, enum, and module conflicts
	// does not detect field, method, generic or scoped-variable conflicts
	symbol_error name_conflicts(const std::string& name);
};