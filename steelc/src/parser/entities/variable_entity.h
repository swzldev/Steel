#include <memory>
#include <string>

#include "entity.h"
#include "../ast/ast_fwd.h"

class variable_entity : public entity, public std::enable_shared_from_this<variable_entity> {
public:
	static std::shared_ptr<variable_entity> make(std::shared_ptr<variable_declaration> declaration);

	std::string name() const override;
	std::string full_name() const override;

	std::shared_ptr<variable_entity> as_variable() override;

	std::shared_ptr<variable_declaration> declaration;

private:
	variable_entity(std::shared_ptr<variable_declaration> declaration)
		: entity(ENTITY_VARIABLE), declaration(declaration) {
	}
};