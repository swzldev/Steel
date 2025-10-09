#include "type_utils.h"

#include "../ast/declarations/type_declaration.h"
#include "data_type.h"
#include "primitive_type.h"
#include "custom_type.h"
#include "container_types.h"

std::vector<std::shared_ptr<function_declaration>> get_method_candidates(std::shared_ptr<const type_declaration> type, const std::string& name, size_t arity) {
	std::vector<std::shared_ptr<function_declaration>> candidates;
	while (type != nullptr) {
		for (const auto& method : type->methods) {
			if (method->identifier == name && method->parameters.size() == arity) {
				candidates.push_back(method);
			}
		}
		type = type->base_class;
	}
	return candidates;
}

data_type_modifier to_type_modifier(token_type tt) {
	data_type_modifier mod;
	switch (tt) {
	default:
		mod = DTM_NONE;
	}
	return mod;
}

primitive to_primitive(token_type tt) {
	primitive dt;
	switch (tt) {
	case TT_I16:
		dt = PRIMITIVE_I16;
		break;
	case TT_I32:
		dt = PRIMITIVE_I32;
		break;
	case TT_I64:
		dt = PRIMITIVE_I64;
		break;
	case TT_FLOAT:
		dt = PRIMITIVE_FLOAT;
		break;
	case TT_DOUBLE:
		dt = PRIMITIVE_DOUBLE;
		break;
	case TT_CHAR:
		dt = PRIMITIVE_CHAR;
		break;
	case TT_STRING:
		dt = PRIMITIVE_STRING;
		break;
	case TT_BYTE:
		dt = PRIMITIVE_BYTE;
		break;
	case TT_BOOL:
		dt = PRIMITIVE_BOOL;
		break;
	case TT_VOID:
		dt = PRIMITIVE_VOID;
		break;
	}
	return dt;
}
data_type_ptr to_data_type(const std::string& type_name) {
	return custom_type::get(type_name);
}

data_type_ptr make_pointer(data_type_ptr base_type) {
	return std::make_shared<pointer_type>(base_type);
}
data_type_ptr make_array(data_type_ptr base_type) {
	return std::make_shared<array_type>(base_type);
}
data_type_ptr make_array(data_type_ptr base_type, size_t size) {
	return std::make_shared<array_type>(base_type, size);
}

bool is_numeric(primitive primitive) {
	switch (primitive) {
	case PRIMITIVE_I16:
	case PRIMITIVE_I32:
	case PRIMITIVE_I64:
	case PRIMITIVE_FLOAT:
	case PRIMITIVE_DOUBLE:
		return true;
	default:
		return false;
	}
}

int size_of_pointer() {
	// TODO: make this based on architecture etc.
	return 8;
}
int size_of_primitive(primitive prim) {
	switch (prim) {
	case PRIMITIVE_I16: return 2;
	case PRIMITIVE_I32: return 4;
	case PRIMITIVE_I64: return 8;
	case PRIMITIVE_FLOAT: return 4;
	case PRIMITIVE_DOUBLE: return 8;
	case PRIMITIVE_CHAR: return 1;
	case PRIMITIVE_STRING: return size_of_pointer();
	case PRIMITIVE_BYTE: return 1;
	case PRIMITIVE_BOOL: return 1;
	case PRIMITIVE_VOID: return 0;
	default: return -1;
	}
}

std::string to_string(primitive primitive) {
	switch (primitive) {
	case PRIMITIVE_I16: return "short";
	case PRIMITIVE_I32: return "int";
	case PRIMITIVE_I64: return "long";
	case PRIMITIVE_FLOAT: return "float";
	case PRIMITIVE_DOUBLE: return "double";
	case PRIMITIVE_CHAR: return "char";
	case PRIMITIVE_STRING: return "string";
	case PRIMITIVE_BYTE: return "byte";
	case PRIMITIVE_BOOL: return "bool";
	case PRIMITIVE_VOID: return "void";
	default: return "unknown";
	}
}