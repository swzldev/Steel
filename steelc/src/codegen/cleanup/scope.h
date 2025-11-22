#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>

#include "cleanup_action.h"

struct scope {
	llvm::BasicBlock* cleanup_block;
	std::vector<cleanup_action> cleanup_actions;
	bool cleanup_emitted = false;

	std::unordered_map<std::string, llvm::AllocaInst*> locals_map;
};