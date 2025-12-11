#pragma once

#include <vector>
#include <memory>
#include <string>

#include <llvm/IR/Module.h>

#include <building/linking/link_error.h>

class project_linker {
public:
	project_linker() = default;
	project_linker(std::vector<std::unique_ptr<llvm::Module>> modules)
		: modules(std::move(modules)) {
	}

	bool load_modules_from_paths(const std::vector<std::string>& paths, bool is_bitcode = true);

	std::unique_ptr<llvm::Module> link_all();

	inline bool has_error() const {
		return last_error != LINK_OK;
	}
	inline link_error get_last_error() const {
		return last_error;
	}
	inline std::string get_error_message() const {
		return error_message;
	}

private:
	llvm::LLVMContext ctx;
	std::vector<std::unique_ptr<llvm::Module>> modules;

	link_error last_error = LINK_OK;
	std::string error_message;

	inline void error(link_error err, const std::string& msg) {
		last_error = err;
		error_message = msg;
	}
};