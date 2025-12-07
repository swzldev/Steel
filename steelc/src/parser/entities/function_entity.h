#include <memory>

#include "entity.h"
#include "../ast/ast_fwd.h"

class function_entity : public entity, public std::enable_shared_from_this<function_entity> {
public:
	static std::shared_ptr<function_entity> make(std::shared_ptr<function_declaration> declaration);

	std::string name() const override;
	std::string full_name() const override;

	std::shared_ptr<function_entity> as_function() override;

	std::shared_ptr<function_declaration> declaration;

private:
	function_entity(std::shared_ptr<function_declaration> declaration)
		: entity(ENTITY_FUNCTION), declaration(declaration) {
	}
};