#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <memory>
#include <utility>
#include <variant>

#include "../ast/ast.h"
#include "../ast/declarations/variable_declaration.h"
#include "../ast/declarations/function_declaration.h"
#include "../ast/declarations/type_declaration.h"
#include "../types/types.h"

enum lookup_error {
	LOOKUP_OK,
	LOOKUP_NO_MATCH,
	LOOKUP_COLLISION,
};

class lookup_result {
public:
	lookup_result() = default;
	lookup_result(lookup_error err)
		: error(err) {
	}
	lookup_result(std::shared_ptr<variable_declaration> var)
		: error(LOOKUP_OK), value(std::move(var)) {
	}
	lookup_result(std::shared_ptr<function_declaration> func)
		: error(LOOKUP_OK), value(std::move(func)) {
	}
	lookup_result(std::shared_ptr<type_declaration> type)
		: error(LOOKUP_OK), value(std::move(type)) {
	}

	lookup_error error = LOOKUP_OK;
	std::variant<
		std::shared_ptr<variable_declaration>,
		std::shared_ptr<function_declaration>,
		std::shared_ptr<type_declaration>
	> value;
};

class symbol_table {
public:
	symbol_table();

	inline bool in_global_scope() const {
		return scopes.size() == 1;
	}
	void push_scope();
	void pop_scope();

	bool add_variable(std::shared_ptr<variable_declaration> var);
	bool add_field(std::shared_ptr<type_declaration> type, std::shared_ptr<variable_declaration> var);
	int add_function(std::shared_ptr<function_declaration> func);
	int add_method(std::shared_ptr<type_declaration> type, std::shared_ptr<function_declaration> func);
	bool add_type(std::shared_ptr<type_declaration> type);

	inline bool has_variable(const std::string& name) const {
		return get_variable(nullptr, name).error == LOOKUP_OK;
	}
	inline bool has_type(const std::string& name) const {
		return types.find(name) != types.end();
	}

	lookup_result get_variable(std::shared_ptr<const type_declaration> type, const std::string& name) const;
	lookup_result get_function(const std::string& name, type_ptr return_type, std::vector<type_ptr> param_types) const;
	lookup_result get_type(const std::string& name) const;
	std::vector<std::shared_ptr<function_declaration>> get_function_candidates(const std::string& name, size_t arity, size_t generics) const;
	
private:
	std::vector<std::map<std::string, std::shared_ptr<variable_declaration>>> scopes;
	std::vector<std::pair<std::string, std::shared_ptr<function_declaration>>> functions;
	std::unordered_map<std::string, std::shared_ptr<type_declaration>> types;
};