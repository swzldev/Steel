#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include <llvm/IR/BasicBlock.h>

#include "cleanup_action.h"
#include "../memory/variable.h"

struct scope {
	llvm::BasicBlock* cleanup_block;
	std::vector<cleanup_action> cleanup_actions;

	bool cleanup_emitted = false;

	std::unordered_map<std::string, std::shared_ptr<variable>> locals_map;
};