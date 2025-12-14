#include "variable_entity.h"

#include <memory>
#include <string>
#include <unordered_map>

#include <ast/declarations/variable_declaration.h>

std::shared_ptr<variable_entity> variable_entity::get(std::shared_ptr<variable_declaration> declaration) {
	static std::unordered_map<std::shared_ptr<variable_declaration>, std::shared_ptr<variable_entity>> cache;
	auto it = cache.find(declaration);
	if (it != cache.end()) {
		return it->second;
	}
	return std::shared_ptr<variable_entity>(new variable_entity(declaration));
}

std::string variable_entity::name() const {
	return declaration->identifier;
}
std::string variable_entity::full_name() const {
	// might change this in the future for scopes
	// variables e.g. module::variable
	return declaration->identifier;
}

std::shared_ptr<variable_entity> variable_entity::as_variable() {
	return shared_from_this();
}

bool variable_entity::is_const() const {
	return declaration->is_const;
}
type_ptr variable_entity::var_type() const {
	return declaration->type;
}
