#include "lvalue.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

llvm::Value* lvalue::load(llvm::IRBuilder<>& builder) {
	return builder.CreateLoad(type, addr);
}
void lvalue::store(llvm::IRBuilder<>& builder, llvm::Value* val) {
	builder.CreateStore(val, addr);
}
