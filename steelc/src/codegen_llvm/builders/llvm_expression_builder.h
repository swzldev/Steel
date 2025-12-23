#pragma once

#include <LLVM/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

#include <lexer/token_type.h>

enum llvm_bin_op {
	ADD,
	SUB,
	MUL,
	DIV,
	REM,
	EQUAL,
	NOT_EQUAL,
	LESS,
	LESS_EQ,
	GREATER,
	GREATER_EQ,
	AND,
	OR,
};

class llvm_expression_builder {
public:
	llvm_expression_builder(llvm::IRBuilder<>& builder)
		: builder(builder), ctx(builder.getContext()) {
	}

	llvm::Value* build_binary_expr(llvm::Value* lhs, llvm::Value* rhs, llvm_bin_op operation);
	llvm::Value* build_unary_expr(llvm::Type* expr_type, llvm::Value* operand, token_type operation);
	// notes:
	// elem_type is the type of the elements being indexed, e.g. int[] -> int
	// this function does no conversion or type checking on base_ptr or index.
	llvm::Value* build_index_expr(llvm::Type* elem_type, llvm::Value* base_ptr, llvm::Value* index);
	llvm::Value* build_struct_init(llvm::Type* struct_type, const std::vector<llvm::Value*>& elements);
	// note: struct_type is contexted to llvm structs, it can be class, struct, etc.
	// this function works for struct pointers and struct values.
	llvm::Value* build_field_access(llvm::Type* struct_type, llvm::Value* object, unsigned field_index);

private:
	llvm::IRBuilder<>& builder;
	llvm::LLVMContext& ctx;

	llvm::Value* build_integer_binary_expr(llvm::Value* lhs, llvm::Value* rhs, llvm_bin_op operation);
	llvm::Value* build_float_binary_expr(llvm::Value* lhs, llvm::Value* rhs, llvm_bin_op operation);
};