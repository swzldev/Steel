#pragma once

#include "data_type.h"
#include "../ast/ast_fwd.h"

enum primitive;

std::vector<std::shared_ptr<function_declaration>> get_method_candidates(std::shared_ptr<const type_declaration> type, const std::string& name, size_t arity);

enum data_type_modifier to_type_modifier(token_type tt);
enum primitive to_primitive(token_type tt);
data_type_ptr to_data_type(const std::string& type_name);

data_type_ptr make_pointer(data_type_ptr base_type);
data_type_ptr make_array(data_type_ptr base_type);
data_type_ptr make_array(data_type_ptr base_type, size_t size);

bool is_numeric(primitive primitive);

int size_of_pointer();
int size_of_primitive(primitive primitive);

std::string to_string(primitive primitive);