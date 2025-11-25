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

private:
	llvm::IRBuilder<>& builder;
	llvm::LLVMContext& ctx;
};