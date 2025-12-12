#include "llvm_expression_builder.h"

#include <llvm/IR/Value.h>

#include <codegen/error/codegen_exception.h>
#include <lexer/token_type.h>

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
llvm::Value* llvm_expression_builder::build_unary_expr(llvm::Type* expr_type, llvm::Value* operand, token_type operation) {
	switch (operation) {
	case TT_NOT:
		return builder.CreateNot(operand);
	case TT_INCREMENT: {
		auto old = builder.CreateLoad(expr_type, operand, "old");
		llvm::Value* one = llvm::ConstantInt::get(expr_type, 1);
		auto inc = builder.CreateAdd(old, one, "inc");
		builder.CreateStore(inc, operand);
		return old;
	}
	case TT_DECREMENT: {
		auto old = builder.CreateLoad(expr_type, operand, "old");
		llvm::Value* one = llvm::ConstantInt::get(expr_type, -1);
		auto inc = builder.CreateAdd(old, one, "inc");
		builder.CreateStore(inc, operand);
		return old;
	}

	default:
		throw codegen_exception("Unsupported unary operation in LLVM expression builder");
	}
}
llvm::Value* llvm_expression_builder::build_index_expr(llvm::Type* elem_type, llvm::Value* base_ptr, llvm::Value* index) {
	if (elem_type->isArrayTy()) {
		llvm::Value* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
		llvm::Value* indices[] = { zero, index };
		return builder.CreateGEP(elem_type, base_ptr, indices, "array.index");
	}

	return builder.CreateGEP(elem_type, base_ptr, index, "ptr.index");
}
llvm::Value* llvm_expression_builder::build_struct_init(llvm::Type* struct_type, const std::vector<llvm::Value*>& elements) {
	llvm::Value* agg = llvm::UndefValue::get(struct_type);

	for (unsigned i = 0; i < elements.size(); ++i) {
		agg = builder.CreateInsertValue(agg, elements[i], { i });
	}

	return agg;
}
llvm::Value* llvm_expression_builder::build_field_access(llvm::Type* struct_type, llvm::Value* object, unsigned field_index) {
	llvm::Type* obj_type = object->getType();

	cg_assert(struct_type->isStructTy(), "Field access on non-struct type in LLVM expression builder");

	if (obj_type->isPointerTy()) {
		return builder.CreateStructGEP(struct_type, object, field_index, "field.access");
	}
	return builder.CreateExtractValue(object, { field_index }, "field.access");
}
