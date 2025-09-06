#pragma once

#include <string>
#include <memory>

#include "../lexer/token.h"
#include "../lexer/token_type.h"

using type_ptr = std::shared_ptr<class data_type>;

enum data_type_modifier to_type_modifier(token_type tt);
enum primitive_type to_primitive(token_type tt);
type_ptr to_data_type(token& tk);
type_ptr to_data_type(token_type tt);
type_ptr to_data_type(primitive_type pt);
type_ptr to_data_type(const std::string& type_name);

type_ptr make_ptr(type_ptr base_type);
type_ptr make_array(type_ptr base_type);
type_ptr make_array(type_ptr base_type, size_t size);

bool is_numeric(enum primitive_type primitive);

int primitive_size_of(enum primitive_type primitive);

std::string to_string(enum primitive_type primitive);