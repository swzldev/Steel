#pragma once

#include <memory>
#include <string>

#include "lookup_result.h"
#include "../ast/ast_fwd.h"
#include "../types/types_fwd.h"
#include "../entities/entities_fwd.h"

// SYMBOL RESOLVER
// - File specific

class import_table;

class symbol_resolver {
public:
	symbol_resolver() = default;

	lookup_result lookup(const std::string& name, const std::shared_ptr<type_entity>& type = nullptr) const;

	std::shared_ptr<module_entity> current_module;
	std::shared_ptr<import_table> import_tbl;
};