#pragma once

#include <string>
#include <memory>

#include "../ast_node.h"

typedef unsigned int declaration_modifiers;
enum decleration_modifier {
	DM_NONE = 0,
};

struct module_info;

class declaration : public ast_node {
public:
	std::shared_ptr<module_info> parent_module = nullptr;
	declaration_modifiers mods = DM_NONE;
	std::string filename;
};