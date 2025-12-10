#pragma once

#include <string>
#include <memory>

#include "../lexer/token.h"
#include "../lexer/token_type.h"

using type_ptr = std::shared_ptr<class data_type>;

enum data_type_kind;

enum data_type_modifier to_type_modifier(token_type tt);
enum data_type_kind to_primitive(token_type tt);
const type_ptr to_data_type(token& tk);
const type_ptr to_data_type(token_type tt);
const type_ptr to_data_type(data_type_kind pt);
const type_ptr to_data_type(const std::string& type_name);

bool is_numeric(data_type_kind primitive);