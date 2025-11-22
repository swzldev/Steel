#pragma once

#include <memory>

#include <llvm/IR/Function.h>

#include "../type_handling/llvm_type_converter.h"
#include "../../parser/ast/declarations/function_declaration.h"

class llvm_function_builder {
public:
	llvm_function_builder(llvm::Module& module, llvm_type_converter& type_converter)
		: module(module), type_converter(type_converter) {
	}

	llvm::Function* build(std::shared_ptr<function_declaration> func_ast);

private:
	llvm::Module& module;

	llvm_type_converter& type_converter;
};