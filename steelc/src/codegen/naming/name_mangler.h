#pragma once

#include <string>
#include <memory>

#include "../../parser/types/data_type.h"
#include "../../parser/ast/declarations/function_declaration.h"

// name mangler
// the name mangler in steel follows the itanium c++ abi mangling scheme
// this is to ensure compatibility with other languages and tools that are built
// around it

class name_mangler {
public:
	std::string mangle_function(std::shared_ptr<function_declaration> func_ast);

private:
	std::string mangle_text(const std::string& text);
	std::string mangle_type(type_ptr t);
};