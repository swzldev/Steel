#pragma once

#include <LLVM/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

#include "../../lexer/token_type.h"

class llvm_expression_builder {
public:
	llvm_expression_builder(llvm::IRBuilder<>& builder)
		: builder(builder), ctx(builder.getContext()) {
	}

	llvm::Value* build_binary_expr(llvm::Value* lhs, llvm::Value* rhs, token_type operation);
	llvm::Value* build_unary_expr(llvm::Type* expr_type, llvm::Value* operand, token_type operation);
	// notes:
	// elem_type is the type of the elements being indexed, e.g. int[] -> int
	// this function does no conversion or type checking on base_ptr or index.
	llvm::Value* build_index_expr(llvm::Type* elem_type, llvm::Value* base_ptr, llvm::Value* index);

private:
	llvm::IRBuilder<>& builder;
	llvm::LLVMContext& ctx;
};