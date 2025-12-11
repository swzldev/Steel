#pragma once

#include <memory>

#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/DerivedTypes.h>

#include <codegen/naming/name_mangler.h>
#include <codegen/type_handling/llvm_type_converter.h>
#include <ast/declarations/function_declaration.h>

class llvm_function_builder {
public:
	llvm_function_builder(llvm::Module& module, llvm_type_converter& type_converter)
		: module(module), type_converter(type_converter) {
	}

	llvm::Function* build(std::shared_ptr<function_declaration> func_ast);
	llvm::FunctionType* get_llvm_fn_type(std::shared_ptr<function_declaration> func_ast);

private:
	static name_mangler mangler;

	llvm::Module& module;

	llvm_type_converter& type_converter;
};