#pragma once

#include <vector>

#include "data_type.h"
#include "../ast/declarations/operator_declaration.h"
#include "../ast/declarations/conversion_declaration.h"
#include "../../lexer/token_type.h"

std::vector<conversion_declaration> get_core_conversions(primitive_type from = DT_UNKNOWN);
std::vector<operator_declaration> get_core_operators(token_type filter = TT_UNKNOWN);