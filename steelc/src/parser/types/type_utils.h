#pragma once

#include "data_type.h"
#include "../ast/ast_fwd.h"

std::vector<std::shared_ptr<function_declaration>> get_method_candidates(std::shared_ptr<const type_declaration> type, const std::string& name, size_t arity);
std::vector<std::shared_ptr<function_declaration>> get_ctor_candidates(std::shared_ptr<const type_declaration> type, size_t arity);

int size_of_primitive(primitive_type primitive);
int size_of_pointer();

type_ptr make_pointer(type_ptr base_type);
type_ptr make_array(type_ptr base_type);
type_ptr make_array(type_ptr base_type, size_t size);

std::string to_string(primitive_type primitive);