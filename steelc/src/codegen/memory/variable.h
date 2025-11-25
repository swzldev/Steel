#pragma once

#include <string>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>

#include "lvalue.h"

class variable : public lvalue {
public:
	variable(const std::string& name, llvm::AllocaInst* slot)
		: lvalue(slot, slot->getAllocatedType()), name(name), slot(slot) {
	}

	inline const std::string& get_name() {
		return name;
	}
	inline llvm::AllocaInst* get_slot() {
		return slot;
	}

private:
	std::string name;
	llvm::AllocaInst* slot;
};