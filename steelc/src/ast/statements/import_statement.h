#pragma once

#include <string>
#include <vector>

#include <ast/ast_node.h>

class import_statement : public ast_node, public std::enable_shared_from_this<import_statement> {
public:
	ENABLE_ACCEPT(import_statement)

	import_statement(std::vector<std::string> module_path)
		: module_path(module_path) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Import Statement: " + module_name + "\n";
	}

	std::vector<std::string> module_path;
};