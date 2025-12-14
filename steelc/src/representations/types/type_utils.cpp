#include "type_utils.h"

#include <representations/types/data_type.h>
#include <representations/types/custom_type.h>
#include <representations/types/container_types.h>
#include <ast/declarations/type_declaration.h>

std::vector<std::shared_ptr<function_declaration>> get_method_candidates(std::shared_ptr<const type_declaration> type, const std::string& name, size_t arity) {
	std::vector<std::shared_ptr<function_declaration>> candidates;
	while (type != nullptr) {
		for (const auto& method : type->methods) {
			if (method->identifier == name && method->parameters.size() == arity) {
				candidates.push_back(method);
			}
		}
		type = type->base_type;
	}
	return candidates;
}

std::vector<std::shared_ptr<function_declaration>> get_ctor_candidates(std::shared_ptr<const type_declaration> type, size_t arity) {
	std::vector<std::shared_ptr<function_declaration>> candidates;

	for (const auto& ctor : type->constructors) {
		if (ctor->parameters.size() == arity) {
			candidates.push_back(ctor);
		}
	}
	return candidates;
}

int size_of_primitive(data_type_kind primitive) {
	switch (primitive) {
	case DT_I16: return 2;
	case DT_I32: return 4;
	case DT_I64: return 8;
	case DT_FLOAT: return 4;
	case DT_DOUBLE: return 8;
	case DT_CHAR: return 1;
	case DT_WIDECHAR: return 2;
	case DT_STRING: return size_of_pointer();
	case DT_WIDESTRING: return size_of_pointer();
	case DT_BYTE: return 1;
	case DT_BOOL: return 1;
	case DT_VOID: return 0;
	default: return -1;
	}
}
int size_of_pointer() {
	// TODO: update this to reflect architecture
	return 8;
}

type_ptr make_pointer(type_ptr base_type) {
	return std::make_shared<pointer_type>(base_type);
}
type_ptr make_array(type_ptr base_type) {
	return std::make_shared<array_type>(base_type);
}
type_ptr make_array(type_ptr base_type, std::shared_ptr<expression> size_expr) {
	return std::make_shared<array_type>(base_type, size_expr);
}

bool derives_from(type_ptr type, type_ptr base) {
	if (!type->is_custom() || !base->is_custom()) {
		return false;
	}

	std::shared_ptr<type_declaration> custom = type->as_custom()->declaration;

	do {
		if (custom->type() == base) {
			return true;
		}
		custom = custom->base_type;
	} while (custom != nullptr);

	return false;
}

std::string to_string(data_type_kind primitive) {
	switch (primitive) {
	case DT_I16: return "small";
	case DT_I32: return "int";
	case DT_I64: return "large";
	case DT_FLOAT: return "float";
	case DT_DOUBLE: return "double";
	case DT_CHAR: return "char";
	case DT_WIDECHAR: return "wchar";
	case DT_STRING: return "string";
	case DT_WIDESTRING: return "wstring";
	case DT_BYTE: return "byte";
	case DT_BOOL: return "bool";
	case DT_VOID: return "void";
	default: return "unknown";
	}
}