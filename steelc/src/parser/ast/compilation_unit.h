#pragma once

#include <string>
#include <memory>
#include <vector>

#include "ast_node.h"
#include "../symbolics/import_table.h"
#include "../../stproj/source_file.h"

class compilation_unit : public ast_node, public std::enable_shared_from_this<compilation_unit> {
public:
	ENABLE_ACCEPT(compilation_unit)

	compilation_unit() {
		static unsigned int next_unit_id = 0;
		unit_id = next_unit_id++;
	}

	std::string string(int indent = 0) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Program:\n";
		if (declarations.size() <= 0) {
			result += ind + "  <Empty>\n";
			return result;
		}
		for (int i = 0; i < declarations.size(); i++) {
			result += ind + declarations[i]->string(indent + 1) + "\n";
		}
		return result;
	}

	std::shared_ptr<source_file> source_file;
	std::vector<ast_node_ptr> declarations;
	import_table import_tbl;
	unsigned int unit_id;
};