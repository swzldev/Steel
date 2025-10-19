#pragma once

#include <string>
#include <memory>

#include "../lexer/token.h"
#include "../lexer/token_type.h"

using type_ptr = std::shared_ptr<class data_type>;

enum primitive_type;

enum data_type_modifier to_type_modifier(token_type tt);
enum primitive_type to_primitive(token_type tt);
const type_ptr to_data_type(token& tk);
const type_ptr to_data_type(token_type tt);
const type_ptr to_data_type(primitive_type pt);
const type_ptr to_data_type(const std::string& type_name);

bool is_numeric(primitive_type primitive);