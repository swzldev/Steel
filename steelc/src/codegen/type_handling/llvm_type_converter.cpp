#include "llvm_type_converter.h"

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

#include "../../parser/types/data_type.h"

llvm::Type* llvm_type_converter::convert(type_ptr t) {
	if (t->is_primitive()) {
		return get_primitive_type(t);
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
