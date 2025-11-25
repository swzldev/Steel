#include "name_mangler.h"

#include <string>
#include <memory>

#include "../../parser/types/data_type.h"
#include "../../parser/ast/declarations/function_declaration.h"

std::string name_mangler::mangle_function(std::shared_ptr<function_declaration> func_ast) {
	std::string mangled = "_Z";
	if (func_ast->is_method) {

	}
	else if (func_ast->is_constructor) {
	
	}
	else {
		mangled += mangle_text(func_ast->identifier);
	}

	return mangled;
}

std::string name_mangler::mangle_text(const std::string& text) {
	// length prefixed mangling, e.g. foo -> 3foo
	return std::to_string(text.length()) + text;
}
std::string name_mangler::mangle_type(type_ptr t) {
	if (t->is_primitive()) {
		switch (t->primitive) {
			
		}
	}

	throw; // should be unreachable, so this is a very unsafe throw
	return "";
}
