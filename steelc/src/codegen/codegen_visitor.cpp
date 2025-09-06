#include "codegen_visitor.h"

#include <string>
#include <memory>

#include "../parser/ast/ast.h"

void codegen_visitor::visit(std::shared_ptr<binary_expression> expr) {
	expr->left->accept(*this);
	auto left = last_value;
	expr->right->accept(*this);
	auto right = last_value;
	switch (expr->oparator) {
	case TT_ADD:
		last_value = builder.CreateAdd(left, right, "addtmp");
		break;
	case TT_SUBTRACT:
		last_value = builder.CreateSub(left, right, "subtmp");
		break;
	case TT_MULTIPLY:
		last_value = builder.CreateMul(left, right, "multmp");
		break;
	case TT_DIVIDE:
		last_value = builder.CreateSDiv(left, right, "divtmp");
		break;
	default:
		last_value = nullptr;
		break;
	}
}
void codegen_visitor::visit(std::shared_ptr<literal> literal) {
	auto value = generate_literal(literal);
	last_value = value;
}

llvm::Value* codegen_visitor::generate_literal(std::shared_ptr<literal> lit) {
	switch (lit->primitive) {
	case DT_I16: {
		short value = std::stoi(lit->value);
		return llvm::ConstantInt::get(context, llvm::APInt(16, value, true));
	}
	case DT_I32: {
		int value = std::stoi(lit->value);
		return llvm::ConstantInt::get(context, llvm::APInt(32, value, true));
	}
	case DT_I64: {
		long long value = std::stoll(lit->value);
		return llvm::ConstantInt::get(context, llvm::APInt(64, value, true));
	}
	case DT_BOOL: {
		return llvm::ConstantInt::get(context, llvm::APInt(1, lit->value == "true" ? 1 : 0, false));
	}
	case DT_FLOAT: {
		float fvalue = std::stof(lit->value);
		return llvm::ConstantFP::get(context, llvm::APFloat(fvalue));
	}
	case DT_DOUBLE: {
		double dvalue = std::stod(lit->value);
		return llvm::ConstantFP::get(context, llvm::APFloat(dvalue));
	}
	case DT_STRING: {
		return llvm::ConstantDataArray::getString(context, lit->value);
	}
	case DT_CHAR: {
		char cvalue = lit->value[0];
		return llvm::ConstantInt::get(context, llvm::APInt(8, cvalue, false));
	}
	case DT_BYTE: {
		unsigned char bvalue = static_cast<unsigned char>(std::stoi(lit->value));
		return llvm::ConstantInt::get(context, llvm::APInt(8, bvalue, false));
	}
	}
}
