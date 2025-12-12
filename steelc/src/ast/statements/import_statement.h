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
		std::string ind = indent_s(indent);
		std::string result = ind + "Import Statement: ";
		for (size_t i = 0; i < module_path.size(); ++i) {
			result += module_path[i];
			if (i < module_path.size() - 1) {
				result += "::";
			}
		}
		return result;
	}

	std::vector<std::string> module_path;
};