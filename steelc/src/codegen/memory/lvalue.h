#pragma once

#include <memory>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>

class lvalue {
public:
	lvalue(llvm::Value* a, llvm::Type* t)
		: addr(a), type(t) {
	}

	inline llvm::Value* address() const {
		return addr;
	}
	inline bool valid() const {
		return addr != nullptr;
	}
	inline llvm::Type* get_type() const {
		return type;
	}

	llvm::Value* load(llvm::IRBuilder<>& builder);
	void store(llvm::IRBuilder<>& builder, llvm::Value* val);

private:
	llvm::Value* addr;
	llvm::Type* type;
};