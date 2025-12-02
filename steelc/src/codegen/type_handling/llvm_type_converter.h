#pragma once

#include <llvm/IR/Type.h>
#include <llvm/IR/LLVMContext.h>

#include "../../parser/types/data_type.h"

class codegen_visitor;

class llvm_type_converter {
public:
	llvm_type_converter(llvm::LLVMContext& context)
		: context(context) {
	}

	llvm::Type* convert(type_ptr t);

private:
	llvm::LLVMContext& context;

	llvm::Type* get_primitive_type(type_ptr t);
	unsigned long long get_array_size(type_ptr t);
};