#pragma once

#include <llvm/IR/Type.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>

using namespace llvm;

class llvm_type_utils {
public:
	static Type* get_i8_ptr_type(LLVMContext& context) {
		return PointerType::getUnqual(Type::getInt8Ty(context));
	}
};