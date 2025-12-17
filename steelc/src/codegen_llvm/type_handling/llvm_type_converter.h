#pragma once

#include <unordered_map>
#include <string>

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>

#include <mir/mir_type.h>

class codegen_visitor;

class llvm_type_converter {
public:
	llvm_type_converter(llvm::LLVMContext& context)
		: context(context) {
	}

	llvm::Type* convert(const mir_type& ty);

private:
	llvm::LLVMContext& context;
	std::unordered_map<std::string, llvm::StructType*> struct_type_cache;

	llvm::Type* get_primitive_type(const mir_type& ty);
	unsigned long long get_array_size(const mir_type& ty);
};