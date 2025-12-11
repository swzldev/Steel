#pragma once

#include <string>
#include <memory>

#include <lexer/token.h>
#include <lexer/token_type.h>
#include <representations/types/types_fwd.h>

enum data_type_kind;

enum data_type_modifier to_type_modifier(token_type tt);
enum data_type_kind to_primitive(token_type tt);
const type_ptr to_data_type(token& tk);
const type_ptr to_data_type(token_type tt);
const type_ptr to_data_type(data_type_kind pt);
const type_ptr to_data_type(const std::string& type_name);
