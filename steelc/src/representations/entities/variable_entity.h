#pragma once

#include <memory>
#include <string>

#include <representations/entities/entity.h>
#include <representations/types/types_fwd.h>
#include <ast/ast_fwd.h>

class variable_entity : public entity, public std::enable_shared_from_this<variable_entity> {
public:
	static std::shared_ptr<variable_entity> make(std::shared_ptr<variable_declaration> declaration);

	std::string name() const override;
	std::string full_name() const override;

	std::shared_ptr<variable_entity> as_variable() override;

	// shorthands
	bool is_const() const;
	type_ptr var_type() const;

	std::shared_ptr<variable_declaration> declaration;

private:
	variable_entity(std::shared_ptr<variable_declaration> declaration)
		: entity(ENTITY_VARIABLE), declaration(declaration) {
	}
};