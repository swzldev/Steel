#include "type_symbols.h"

#include <string>
#include <memory>
#include <vector>

#include "../entities/variable_entity.h"
#include "../entities/function_entity.h"

symbol_error type_symbols::add_field(const std::string& name, std::shared_ptr<variable_entity> field) {
	// check if already exists
    if (fields.find(name) != fields.end()) {
        return SYMBOL_CONFLICTS_WITH_FIELD;
	}
	// check if name conflicts with method
    if (methods.find(name) != methods.end()) {
        return SYMBOL_CONFLICTS_WITH_METHOD;
	}

    fields[name] = field;
	return SYMBOL_OK;
}
symbol_error type_symbols::add_method(const std::string& name, std::shared_ptr<function_entity> method) {
	// check if conflicts with field
    if (fields.find(name) != fields.end()) {
        return SYMBOL_CONFLICTS_WITH_FIELD;
    }

	// we dont care if method already exists, overloads allowed
	methods[name].push_back(method);
	return SYMBOL_OK;
}

std::shared_ptr<variable_entity> type_symbols::get_field(const std::string& name) const {
	auto it = fields.find(name);
	if (it != fields.end()) {
		return it->second;
	}
	return nullptr;
}
std::vector<std::shared_ptr<function_entity>> type_symbols::get_methods(const std::string& name) const {
	auto it = methods.find(name);
	if (it != methods.end()) {
		return it->second;
	}
	return {};
}
