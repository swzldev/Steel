#include "llvm_expression_builder.h"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

#include <codegen/error/codegen_exception.h>
#include <lexer/token_type.h>

llvm::Value* llvm_expression_builder::build_binary_expr(llvm::Value* lhs, llvm::Value* rhs, llvm_bin_op operation) {
	llvm::Type* lty = lhs->getType();
	llvm::Type* rty = rhs->getType();
	cg_assert(lty == rty, "Mismatched types in LLVM binary expression builder");

	bool is_integer_type = lty->isIntegerTy();
	bool is_floating_type = lty->isFloatingPointTy();

	if (is_integer_type) {
		return build_integer_binary_expr(lhs, rhs, operation);
	}
	else if (is_floating_type) {
		return build_float_binary_expr(lhs, rhs, operation);
	}
	else {
		throw codegen_exception("Unsupported types in LLVM binary expression builder");
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

llvm::Value* llvm_expression_builder::build_integer_binary_expr(llvm::Value* lhs, llvm::Value* rhs, llvm_bin_op operation) {
	if (!lhs->getType()->isIntegerTy() || !rhs->getType()->isIntegerTy()) {
		throw codegen_exception("Non-integer types in integer binary expression builder");
	}

	switch (operation) {
		// ARITHMETIC
	case llvm_bin_op::ADD:
		return builder.CreateAdd(lhs, rhs);
	case llvm_bin_op::SUB:
		return builder.CreateSub(lhs, rhs);
	case llvm_bin_op::MUL:
		return builder.CreateMul(lhs, rhs);
	case llvm_bin_op::DIV:
		return builder.CreateSDiv(lhs, rhs);
	case llvm_bin_op::REM:
		return builder.CreateSRem(lhs, rhs);

		// COMPARISON
	case llvm_bin_op::EQUAL:
		return builder.CreateICmpEQ(lhs, rhs);
	case llvm_bin_op::NOT_EQUAL:
		return builder.CreateICmpNE(lhs, rhs);
	case llvm_bin_op::LESS:
		return builder.CreateICmpSLT(lhs, rhs);
	case llvm_bin_op::LESS_EQ:
		return builder.CreateICmpSLE(lhs, rhs);
	case llvm_bin_op::GREATER:
		return builder.CreateICmpSGT(lhs, rhs);
	case llvm_bin_op::GREATER_EQ:
		return builder.CreateICmpSGE(lhs, rhs);

	default:
		throw codegen_exception("Unsupported integer binary operation in LLVM expression builder");
	}
}
llvm::Value* llvm_expression_builder::build_float_binary_expr(llvm::Value* lhs, llvm::Value* rhs, llvm_bin_op operation) {
	if (!lhs->getType()->isFloatingPointTy() || !rhs->getType()->isFloatingPointTy()) {
		throw codegen_exception("Non-float types in floating-point binary expression builder");
	}

	switch (operation) {
		// ARITHMETIC
	case llvm_bin_op::ADD:
		return builder.CreateFAdd(lhs, rhs);
	case llvm_bin_op::SUB:
		return builder.CreateFSub(lhs, rhs);
	case llvm_bin_op::MUL:
		return builder.CreateFMul(lhs, rhs);
	case llvm_bin_op::DIV:
		return builder.CreateFDiv(lhs, rhs);
	case llvm_bin_op::REM:
		return builder.CreateFRem(lhs, rhs);

		// COMPARISON
	case llvm_bin_op::EQUAL:
		return builder.CreateICmpEQ(lhs, rhs);
	case llvm_bin_op::NOT_EQUAL:
		return builder.CreateICmpNE(lhs, rhs);
	case llvm_bin_op::LESS:
		return builder.CreateICmpSLT(lhs, rhs);
	case llvm_bin_op::LESS_EQ:
		return builder.CreateICmpSLE(lhs, rhs);
	case llvm_bin_op::GREATER:
		return builder.CreateICmpSGT(lhs, rhs);
	case llvm_bin_op::GREATER_EQ:
		return builder.CreateICmpSGE(lhs, rhs);

	default:
		throw codegen_exception("Unsupported float binary operation in LLVM expression builder");
	}
}
