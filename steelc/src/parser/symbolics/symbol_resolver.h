#pragma once

#include <vector>

#include "import_table.h"
#include "../modules/module_info.h"

// SYMBOL RESOLVER
// - File specific

class symbol_resolver {
public:
	symbol_resolver() = default;

	lookup_result get_variable(std::shared_ptr<const type_declaration> type, const std::string& name) const;
	lookup_result get_function(const std::string& name, type_ptr return_type, std::vector<type_ptr> param_types) const;
	lookup_result get_method(std::shared_ptr<const type_declaration> type, const std::string& name, type_ptr return_type, std::vector<type_ptr> param_types) const;
	lookup_result get_type(const std::string& name) const;
	std::vector<std::shared_ptr<function_declaration>> get_function_candidates(const std::string& name, size_t arity, size_t generics = 0) const;
	std::vector<std::shared_ptr<function_declaration>> get_ctor_candidates(const std::string& name, size_t arity);

	std::shared_ptr<module_info> current_module;
	std::shared_ptr<import_table> import_tbl;
};