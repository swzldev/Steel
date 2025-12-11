#pragma once

#include <string>
#include <memory>

#include <ast/declarations/declaration.h>
#include <representations/entities/entities_fwd.h>

class module_declaration : public declaration, public std::enable_shared_from_this<module_declaration> {
public:
	ENABLE_ACCEPT(module_declaration)

	module_declaration(std::string name, std::vector<ast_ptr> declarations)
		: name(name), declarations(declarations), entity(nullptr) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Module Declaration: \"" + name + "\"\n";
		return result;
	}

	std::string name;
	std::vector<ast_ptr> declarations;
	std::shared_ptr<module_entity> entity;
};