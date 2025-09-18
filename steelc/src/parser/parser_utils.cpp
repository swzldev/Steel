#include "parser_utils.h"

#include "types/types.h"
#include "types/custom_types.h"
#include "types/container_types.h"

data_type_modifier to_type_modifier(token_type tt) {
	data_type_modifier mod;
	switch (tt) {
	case TT_CONST:
		mod = DTM_CONST;
		break;
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
std::shared_ptr<class data_type> to_data_type(token& tk) {
	return to_primitive(tk.type) != DT_UNKNOWN ? std::make_shared<data_type>(to_primitive(tk.type)) : to_data_type(tk.value);
}
type_ptr to_data_type(token_type tt) {
	primitive_type prim = to_primitive(tt);
	if (prim != DT_UNKNOWN) {
		return std::make_shared<data_type>(prim);
	}
	// custom type
	return nullptr;
}
type_ptr to_data_type(primitive_type pt) {
	if (pt != DT_UNKNOWN) {
		return std::make_shared<data_type>(pt);
	}
	return nullptr;
}

std::shared_ptr<class data_type> to_data_type(const std::string& type_name) {
	return custom_type::get(type_name);
}

type_ptr make_pointer(type_ptr base_type) {
	return std::make_shared<pointer_type>(base_type);
}
type_ptr make_array(type_ptr base_type) {
	return std::make_shared<array_type>(base_type);
}
type_ptr make_array(type_ptr base_type, size_t size) {
	return std::make_shared<array_type>(base_type, size);
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

int primitive_size_of(primitive_type primitive) {
	switch (primitive) {
	case DT_I16: return 2;
	case DT_I32: return 4;
	case DT_I64: return 8;
	case DT_FLOAT: return 4;
	case DT_DOUBLE: return 8;
	case DT_CHAR: return 1;
	case DT_STRING: return 8;
	case DT_BYTE: return 1;
	case DT_BOOL: return 1;
	case DT_VOID: return 0;
	default: return -1;
	}
}

std::string to_string(primitive_type primitive) {
	switch (primitive) {
	case DT_I16: return "small";
	case DT_I32: return "int";
	case DT_I64: return "large";
	case DT_FLOAT: return "float";
	case DT_DOUBLE: return "double";
	case DT_CHAR: return "char";
	case DT_STRING: return "string";
	case DT_BYTE: return "byte";
	case DT_BOOL: return "bool";
	case DT_VOID: return "void";
	default: return "unknown";
	}
}