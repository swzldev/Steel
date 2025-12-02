#include "llvm_type_converter.h"

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

#include "../codegen_visitor.h"
#include "../../parser/types/data_type.h"
#include "../../parser/types/container_types.h"

llvm::Type* llvm_type_converter::convert(type_ptr t) {
	if (t->is_primitive()) {
		return get_primitive_type(t);
	}
	else if (auto arr = t->as_array()) {
		llvm::Type* element_type = convert(arr->base_type);

		// TODO
		//return llvm::ArrayType::get(element_type, arr->size);
	}
	else if (auto ptr = t->as_pointer()) {
		auto base_type = convert(ptr->base_type);
		return llvm::PointerType::getUnqual(base_type);
	}

	return nullptr;
}

llvm::Type* llvm_type_converter::get_primitive_type(type_ptr t) {
	switch (t->primitive) {
		case DT_I16:
			return (llvm::Type*)llvm::Type::getInt16Ty(context);
		case DT_I32:
			return (llvm::Type*)llvm::Type::getInt32Ty(context);
		case DT_I64:
			return (llvm::Type*)llvm::Type::getInt64Ty(context);
		case DT_FLOAT:
			return llvm::Type::getFloatTy(context);
		case DT_DOUBLE:
			return llvm::Type::getDoubleTy(context);
		case DT_CHAR:
			return (llvm::Type*)llvm::Type::getInt8Ty(context);
		case DT_STRING:
			// strings are represented as i8*
			return llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context));
		case DT_BOOL:
			return (llvm::Type*)llvm::Type::getInt1Ty(context);
		case DT_VOID:
			return llvm::Type::getVoidTy(context);
		default:
			return nullptr;
	}
}
unsigned long long llvm_type_converter::get_array_size(type_ptr t) {
	auto arr = t->as_array();
	if (!arr) {
		throw; // shouldnt be possible
	}

	// TODO
	return 0;
}
