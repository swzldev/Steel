#pragma once

#include <ast/ast_fwd.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>

std::vector<std::shared_ptr<function_declaration>> get_method_candidates(std::shared_ptr<const type_declaration> type, const std::string& name, size_t arity);
std::vector<std::shared_ptr<function_declaration>> get_ctor_candidates(std::shared_ptr<const type_declaration> type, size_t arity);

int size_of_primitive(data_type_kind primitive);
int size_of_pointer();

type_ptr make_pointer(type_ptr base_type);
type_ptr make_array(type_ptr base_type);
type_ptr make_array(type_ptr base_type, std::shared_ptr<expression> size_expr);

// returns true if type derives from base
bool derives_from(type_ptr type, type_ptr base);

std::string to_string(data_type_kind primitive);