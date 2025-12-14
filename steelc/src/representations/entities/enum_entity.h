#pragma once

#include <memory>
#include <string>

#include <representations/entities/entity.h>
#include <ast/ast_fwd.h>

class enum_entity : public entity, public std::enable_shared_from_this<enum_entity> {
public:
	static std::shared_ptr<enum_entity> get(std::shared_ptr<enum_declaration> declaration);

	std::string name() const override;
	std::string full_name() const override;

	std::shared_ptr<enum_entity> as_enum() override;

	std::shared_ptr<enum_declaration> declaration;

private:
	enum_entity(std::shared_ptr<enum_declaration> declaration)
		: entity(ENTITY_ENUM), declaration(declaration) {
	}
};