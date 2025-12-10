#include "variable_entity.h"

#include <memory>
#include <string>

#include "../ast/declarations/variable_declaration.h"

std::shared_ptr<variable_entity> variable_entity::make(std::shared_ptr<variable_declaration> declaration) {
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
