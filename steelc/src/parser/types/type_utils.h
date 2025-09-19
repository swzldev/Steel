#pragma once

#include "types.h"
#include "../ast/ast_fwd.h"

std::vector<std::shared_ptr<function_declaration>> get_method_candidates(std::shared_ptr<const type_declaration> type, const std::string& name, size_t arity);