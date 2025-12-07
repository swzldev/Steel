#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <memory>
#include <utility>

#include "lookup_result.h"
#include "../ast/declarations/variable_declaration.h"
#include "../ast/declarations/function_declaration.h"
#include "../ast/declarations/type_declaration.h"
#include "../../error/error_catalog.h"

struct module_info;

class symbol_table {
public:
	symbol_table();

	inline bool in_global_scope() const {
		return variables.size() == 1;
	}
	void push_scope();
	void pop_scope();

	error_code add_variable(std::shared_ptr<variable_declaration> var);
	error_code add_field(std::shared_ptr<type_declaration> type, std::shared_ptr<variable_declaration> var);
	error_code add_function(std::shared_ptr<function_declaration> func);
	error_code add_method(std::shared_ptr<type_declaration> type, std::shared_ptr<function_declaration> func);
	error_code add_type(std::shared_ptr<type_declaration> type);
	error_code add_enum(std::shared_ptr<enum_declaration> enm);
	error_code add_generic(std::shared_ptr<generic_parameter> param);
	bool add_module(std::shared_ptr<module_info> module);

	inline bool has_variable(const std::string& name) const {
		return get_variable(nullptr, name).found();
	}
	inline bool has_function(const std::string& name) const {
		return get_function(name).found();
	}
	inline bool has_type(const std::string& name) const {
		return types.find(name) != types.end();
	}
	inline bool has_enum(const std::string& name) const {
		return enums.find(name) != enums.end();
	}
	inline bool has_generic(const std::string& name) const {
		return get_generic(name) != nullptr;
	}
	inline bool has_module(const std::string& name) const {
		return modules.find(name) != modules.end();
	}

	lookup_result lookup(const std::string& name) const;

	lookup_result get_variable(std::shared_ptr<const type_declaration> type, const std::string& name) const;
	lookup_result get_function(const std::string& name) const;
	lookup_result get_function(const std::string& name, type_ptr return_type, std::vector<type_ptr> param_types) const;
	lookup_result get_type(const std::string& name) const;
	lookup_result get_enum(const std::string& name) const;
	lookup_result get_module(const std::string& name) const;
	std::shared_ptr<generic_parameter> get_generic(const std::string& name) const;
	std::vector<std::shared_ptr<function_declaration>> get_function_candidates(const std::string& name, size_t arity, size_t generics) const;

private:
	std::vector<std::map<std::string, std::shared_ptr<variable_declaration>>> variables;
	std::vector<std::pair<std::string, std::shared_ptr<function_declaration>>> functions;
	std::unordered_map<std::string, std::shared_ptr<type_declaration>> types;
	std::unordered_map<std::string, std::shared_ptr<enum_declaration>> enums;
	std::vector<std::map<std::string, std::shared_ptr<generic_parameter>>> generic_scopes;
	std::unordered_map<std::string, std::shared_ptr<module_info>> modules;
};