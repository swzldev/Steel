#include "llvm_expression_builder.h"

#include <llvm/IR/Value.h>

#include "../error/codegen_exception.h"
#include "../../lexer/token_type.h"

llvm::Value* llvm_expression_builder::build_binary_expr(llvm::Value* lhs, llvm::Value* rhs, token_type operation) {
	switch (operation) {
	// ARITHMETIC
	case TT_ADD:
		return builder.CreateAdd(lhs, rhs);
	case TT_SUBTRACT:
		return builder.CreateSub(lhs, rhs);
	case TT_MULTIPLY:
		return builder.CreateMul(lhs, rhs);
	case TT_DIVIDE:
		return builder.CreateSDiv(lhs, rhs);
	case TT_MODULO:
		return builder.CreateSRem(lhs, rhs);

	// COMPARISON
	case TT_EQUAL:
		return builder.CreateICmpEQ(lhs, rhs);
	case TT_NOT_EQUAL:
		return builder.CreateICmpNE(lhs, rhs);
	case TT_LESS:
		return builder.CreateICmpSLT(lhs, rhs);
	case TT_LESS_EQ:
		return builder.CreateICmpSLE(lhs, rhs);
	case TT_GREATER:
		return builder.CreateICmpSGT(lhs, rhs);
	case TT_GREATER_EQ:
		return builder.CreateICmpSGE(lhs, rhs);

	// LOGICAL
	case TT_AND:
		return builder.CreateAnd(lhs, rhs);
	case TT_OR:
		return builder.CreateOr(lhs, rhs);

	// BITWISE

	default:
		throw codegen_exception("Unsupported binary operation in LLVM expression builder");
	}

}