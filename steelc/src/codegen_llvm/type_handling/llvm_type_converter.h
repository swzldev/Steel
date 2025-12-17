#pragma once

#include <unordered_map>
#include <string>

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>

#include <representations/types/types_fwd.h>

class codegen_visitor;

class llvm_type_converter {
public:
	llvm_type_converter(llvm::LLVMContext& context)
		: context(context) {
	}

	llvm::Type* convert(type_ptr t);

private:
	llvm::LLVMContext& context;
	std::unordered_map<std::string, llvm::StructType*> struct_type_cache;

	llvm::Type* get_primitive_type(type_ptr t);
	unsigned long long get_array_size(type_ptr t);
};