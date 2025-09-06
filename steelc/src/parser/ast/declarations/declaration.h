#pragma once

#include <string>

#include "../ast_node.h"

typedef unsigned int declaration_modifiers;
enum decleration_modifier {
	DM_NONE = 0,
};

class declaration : public ast_node {
public:
	declaration_modifiers mods = DM_NONE;
	std::string filename;
};