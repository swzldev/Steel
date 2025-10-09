#pragma once

#include "../ast/ast.h"
#include "data_type.h"

class type_handle {
public:
	inline type_handle(ast_ptr<type_node> syntax_type)
		: syntax_type(syntax_type) {
		resolved_type = data_type::UNKNOWN; 
	}

	template<typename ResolveFn>
	inline void resolve(ResolveFn&& resolve_function);

	inline bool is_resolved() const {
		return resolved_type != data_type::UNKNOWN;
	}

	inline data_type_ptr get() const {
		return resolved_type;
	}

private:
	ast_ptr<type_node> syntax_type;
	data_type_ptr resolved_type;
};

template<typename ResolveFn>
inline void type_handle::resolve(ResolveFn&& resolve_function) {
	if (resolved_type) {
		return;
	}
	if (syntax_type) {
		resolved_type = resolve_function(syntax_type);
	}
}
