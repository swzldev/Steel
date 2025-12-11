#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include <symbolics/symbol_error.h>
#include <representations/entities/entities_fwd.h>

class type_symbols {
public:
	type_symbols() = default;
	
	// returns false if already a member with name
	symbol_error add_field(const std::string& name, std::shared_ptr<variable_entity> field);
	symbol_error add_method(const std::string& name, std::shared_ptr<function_entity> method);

	// returns nullptr if not found
	std::shared_ptr<variable_entity> get_field(const std::string& name) const;
	// returns empty vector if not found
	std::vector<std::shared_ptr<function_entity>> get_methods(const std::string& name) const;
	
private:
	std::unordered_map<std::string, std::shared_ptr<variable_entity>> fields;
	std::unordered_map<std::string, std::vector<std::shared_ptr<function_entity>>> methods;
};