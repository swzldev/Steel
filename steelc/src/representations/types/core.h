#pragma once

#include <vector>

#include <lexer/token_type.h>
#include <ast/ast_fwd.h>
#include <representations/types/data_type.h>

std::vector<conversion_declaration> get_core_conversions(data_type_kind from = DT_UNKNOWN);
std::vector<operator_declaration> get_core_operators(token_type filter = TT_UNKNOWN);