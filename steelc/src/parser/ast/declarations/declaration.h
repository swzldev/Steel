#pragma once

#include <string>
#include <vector>

#include "../ast_node.h"

enum declaration_modifier {
	DM_NONE = 0,
};

class declaration : public ast_node {
public:
	std::vector<declaration_modifier> mods;
	std::string filename;
};