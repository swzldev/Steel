#pragma once

#include <memory>
#include <string>

#include <representations/entities/entity.h>
#include <representations/types/types_fwd.h>
#include <ast/ast_fwd.h>
#include <symbolics/type_symbols.h>

class type_entity : public entity, public std::enable_shared_from_this<type_entity> {
public:
	static std::shared_ptr<type_entity> make(std::shared_ptr<type_declaration> declaration);

	std::string name() const override;
	std::string full_name() const override;

	std::shared_ptr<type_entity> as_type() override;

	std::shared_ptr<type_declaration> declaration;
	type_symbols symbols;

private:
	type_entity(std::shared_ptr<type_declaration> declaration)
		: entity(ENTITY_TYPE), declaration(declaration) {
	}
};