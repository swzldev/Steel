#include "parser_utils.h"

#include "types/data_type.h"
#include "types/custom_type.h"
#include "types/container_types.h"

data_type_modifier to_type_modifier(token_type tt) {
	data_type_modifier mod;
	switch (tt) {
	default:
		mod = DTM_NONE;
	}
	return mod;
}

primitive_type to_primitive(token_type tt) {
    primitive_type dt;
    switch (tt) {
	case TT_I16:
		dt = DT_I16;
		break;
	case TT_I32:
		dt = DT_I32;
		break;
	case TT_I64:
		dt = DT_I64;
		break;
	case TT_FLOAT:
		dt = DT_FLOAT;
		break;
	case TT_DOUBLE:
		dt = DT_DOUBLE;
		break;
	case TT_CHAR:
		dt = DT_CHAR;
		break;
	case TT_STRING:
		dt = DT_STRING;
		break;
	case TT_BYTE:
		dt = DT_BYTE;
		break;
	case TT_BOOL:
		dt = DT_BOOL;
		break;
	case TT_VOID:
		dt = DT_VOID;
		break;
	default:
		dt = DT_UNKNOWN;
    }
	return dt;
}
const type_ptr to_data_type(token& tk) {
	auto type = to_primitive(tk.type) != DT_UNKNOWN ? std::make_shared<data_type>(to_primitive(tk.type)) : to_data_type(tk.value);
	type->position = tk.pos;
	return type;
}
const type_ptr to_data_type(token_type tt) {
	primitive_type prim = to_primitive(tt);
	if (prim != DT_UNKNOWN) {
		return std::make_shared<data_type>(prim);
	}
	// custom type
	return nullptr;
}
const type_ptr to_data_type(primitive_type pt) {
	if (pt != DT_UNKNOWN) {
		return std::make_shared<data_type>(pt);
	}
	return nullptr;
}
const type_ptr to_data_type(const std::string& type_name) {
	return custom_type::get(type_name);
}

bool is_numeric(primitive_type primitive) {
	switch (primitive) {
	case DT_I16:
	case DT_I32:
	case DT_I64:
	case DT_FLOAT:
	case DT_DOUBLE:
		return true;
	default:
		return false;
	}
}