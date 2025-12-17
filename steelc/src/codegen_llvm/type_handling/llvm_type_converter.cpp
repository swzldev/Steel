#include "llvm_type_converter.h"

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

#include <codegen/error/codegen_exception.h>
#include <ast/declarations/type_declaration.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/types/custom_type.h>
#include <representations/types/container_types.h>

llvm::Type* llvm_type_converter::convert(const mir_type& ty) {
	auto& t = ty.ty;
	if (t->is_unknown()) {
		throw codegen_exception("Cannot convert unknown type");
	}
	else if (t->is_primitive()) {
		return get_primitive_type(mir_type{ t });
	}
	else if (auto custom = t->as_custom()) {
		std::string custom_name_full = custom->name(); // change this when scoped types are implemented
		if (struct_type_cache.find(custom_name_full) != struct_type_cache.end()) {
			return struct_type_cache[custom_name_full];
		}

		std::vector<llvm::Type*> member_types;
		for (const auto& field : custom->declaration->fields) {
			member_types.push_back(convert(mir_type{ field->type }));
		}

		llvm::StructType* strct = llvm::StructType::create(context, custom->name());
		strct->setBody(member_types);
		struct_type_cache[custom_name_full] = strct;
		return strct;
	}
	else if (auto arr = t->as_array()) {
		llvm::Type* element_type = convert(mir_type{ arr->base_type });

		// TODO
		//return llvm::ArrayType::get(element_type, arr->size);
	}
	else if (auto ptr = t->as_pointer()) {
		auto base_type = convert(mir_type{ ptr->base_type });
		return llvm::PointerType::getUnqual(base_type);
	}

	return nullptr;
}

llvm::Type* llvm_type_converter::get_primitive_type(const mir_type& ty) {
	auto& t = ty.ty;
	switch (t->primitive) {
		case DT_I16:
			return llvm::Type::getInt16Ty(context);
		case DT_I32:
			return llvm::Type::getInt32Ty(context);
		case DT_I64:
			return llvm::Type::getInt64Ty(context);
		case DT_FLOAT:
			return llvm::Type::getFloatTy(context);
		case DT_DOUBLE:
			return llvm::Type::getDoubleTy(context);
		case DT_CHAR:
			return llvm::Type::getInt8Ty(context);
		case DT_STRING:
			// strings are represented as i8*
			return llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context));
		case DT_BOOL:
			return llvm::Type::getInt1Ty(context);
		case DT_VOID:
			return llvm::Type::getVoidTy(context);
		default:
			return nullptr;
	}
}
unsigned long long llvm_type_converter::get_array_size(const mir_type& ty) {
	auto& t = ty.ty;
	auto arr = t->as_array();
	if (!arr) {
		throw codegen_exception("Non-array type passed to get_array_size");
	}

	// TODO
	return 0;
}
