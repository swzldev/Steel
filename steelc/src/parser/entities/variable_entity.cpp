#include "variable_entity.h"

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
	return std::static_pointer_cast<variable_entity>(shared_from_this());
}
