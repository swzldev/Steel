#pragma once

#include <string>
#include <memory>

#include "../ast_fwd.h"

class module_declaration : public declaration, public std::enable_shared_from_this<module_declaration> {
public:
	ENABLE_ACCEPT(module_declaration)

	module_declaration(std::string name, std::vector<ast_ptr<declaration>> declarations)
		: name(name), declarations(declarations), module_info(nullptr) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Module Declaration: \"" + name + "\"\n";
		return result;
	}

	std::string name;
	std::vector<ast_ptr<declaration>> declarations;
	std::shared_ptr<struct module_info> module_info;
};