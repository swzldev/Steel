#pragma once

#include <memory>

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/DerivedTypes.h>

#include <codegen/naming/name_mangler.h>
#include <codegen_llvm/type_handling/llvm_type_converter.h>
#include <mir/mir_function.h>

class llvm_function_builder {
public:
	llvm_function_builder(llvm_type_converter& type_converter)
		: type_converter(type_converter) {
	}

	llvm::Function* build(const mir_function& fn_mir, llvm::Module* module);
	llvm::FunctionType* get_llvm_fn_type(const mir_function& fn_mir);

private:
	static name_mangler mangler;

	llvm_type_converter& type_converter;
};