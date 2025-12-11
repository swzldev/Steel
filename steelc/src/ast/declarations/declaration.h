#pragma once

#include <string>
#include <memory>

#include <ast/ast_node.h>
#include <representations/entities/entities_fwd.h>

typedef unsigned int declaration_modifiers;
enum decleration_modifier {
	DM_NONE = 0,
};

class declaration : public ast_node {
public:
	std::shared_ptr<module_entity> parent_module = nullptr;
	declaration_modifiers mods = DM_NONE;
	std::string filename;
};