#pragma once

#include <string>
#include <memory>

#include "types/type_handle.h"
#include "../lexer/token.h"
#include "../lexer/token_type.h"

type_handle make_type(token& tk);