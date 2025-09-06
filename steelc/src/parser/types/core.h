#pragma once

#include <vector>

#include "types.h"
#include "../ast/declarations/operator_declaration.h"
#include "../ast/declarations/conversion_declaration.h"
#include "../../lexer/token_type.h"

std::vector<conversion_declaration> get_core_conversions(type_ptr from = nullptr);
std::vector<operator_declaration> get_core_operators(token_type filter = TT_UNKNOWN);