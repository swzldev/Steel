#pragma once

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include <codegen/icode_generator.h>
#include <codegen/codegen_result.h>
#include <codegen/codegen_config.h>
#include <codegen_llvm/type_handling/llvm_type_converter.h>
#include <codegen_llvm/builders/llvm_function_builder.h>
#include <codegen_llvm/builders/llvm_expression_builder.h>
#include <codegen_llvm/writers/llvm_writer.h>

namespace llvm {
	class Function;
}

// llvm_code_generator
//
// this is the built in and default LLVM based code generator for steelc

class llvm_code_generator : public icode_generator {
public:
	llvm_code_generator()
		: builder(context),
		ty_converter(context),
		fn_builder(ty_converter),
		expression_builder(builder) {
	}

	codegen_result emit(const mir_module& mod_mir, const codegen_config& cfg) override;

private:
	llvm::LLVMContext context;
	std::unique_ptr<llvm::Module> module = nullptr;
	llvm::IRBuilder<> builder;

	llvm::Function* current_func = nullptr;

	llvm_type_converter ty_converter;
	llvm_function_builder fn_builder;
	llvm_expression_builder expression_builder;
	std::unique_ptr<llvm_writer> writer = nullptr;

	llvm::Function* emit_function(const mir_function& fn_mir);
	llvm::BasicBlock* emit_block(const mir_block& block_mir);
	void emit_instr(const mir_instr& instr_mir);

	// artifact generators
	code_artifact generate_bitcode_artifact(const mir_module& mod_mir);
	code_artifact generate_asm_artifact(const mir_module& mod_mir);
};