#include "llvm_code_generator.h"

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include <codegen/codegen_result.h>
#include <codegen/codegen_config.h>
#include <codegen/error/codegen_exception.h>
#include <codegen_llvm/type_handling/llvm_type_converter.h>
#include <codegen_llvm/builders/llvm_function_builder.h>
#include <codegen_llvm/builders/llvm_expression_builder.h>
#include <codegen_llvm/writers/llvm_writer.h>
#include <mir/mir_module.h>
#include <mir/mir_function.h>
#include <mir/mir_block.h>
#include <mir/mir_instr.h>

codegen_result llvm_code_generator::emit(const mir_module& mod_mir, const codegen_config& cfg) {
	module = std::make_unique<llvm::Module>(mod_mir.name, context);
	writer = std::make_unique<llvm_writer>(module.get());

	for (const auto& fn : mod_mir.functions) {
		emit_function(fn);
	}
	
	// for now generate bitcode by default
	

	codegen_result result;
	if (cfg.emit_llvm_asm) {
		result.artifacts.push_back(generate_asm_artifact(mod_mir));
	}
	if (cfg.emit_llvm_bc) {
		result.artifacts.push_back(generate_bitcode_artifact(mod_mir));
	}
	return result;
}

llvm::Function* llvm_code_generator::emit_function(const mir_function& fn_mir) {
	llvm::Function* fn_llvm = fn_builder.build(fn_mir, module.get());
	current_func = fn_llvm;

	//env = std::make_unique<codegen_env>(*module, *fn_llvm, builder);

	for (const auto& block : fn_mir.blocks) {
		emit_block(block);
	}

	current_func = nullptr;
	return fn_llvm;
}
llvm::BasicBlock* llvm_code_generator::emit_block(const mir_block& block_mir) {
	if (current_func == nullptr) {
		throw codegen_exception("Cannot emit block outside of function");
	}

	llvm::BasicBlock* block_llvm = llvm::BasicBlock::Create(
		/* Context */ context,
		/* Name */ block_mir.name,
		/* Parent Function */ current_func
	);
	builder.SetInsertPoint(block_llvm);

	for (const auto& instr : block_mir.get_instrs()) {
		emit_instr(instr);
	}

	return block_llvm;
}
void llvm_code_generator::emit_instr(const mir_instr& instr_mir) {
	// this is the main translation from MIR to LLVM
	// they are both very similar, so its mostly just a giant switch
	switch (instr_mir.kind) {
	case mir_instr_opcode::NOP: {
		break;
	}
	default:
		throw codegen_exception("Unimplemented MIR instruction opcode in LLVM code generator");
	}

}

code_artifact llvm_code_generator::generate_bitcode_artifact(const mir_module& mod_mir) {
	return code_artifact{
		.type = artifact_type::ARTIFACT_BITCODE,
		.src_relpath = mod_mir.meta.src_relpath,
		.name = mod_mir.name,
		.extension = ".bc",
		.format = "LLVM-BC",
		.bytes = writer->write_bitcode()
	};
}
code_artifact llvm_code_generator::generate_asm_artifact(const mir_module& mod_mir) {
	return code_artifact{
		.type = artifact_type::ARTIFACT_ASM_IR,
		.src_relpath = mod_mir.meta.src_relpath,
		.name = mod_mir.name,
		.extension = ".ll",
		.format = "LLVM-IR",
		.text = writer->write_asm()
	};
}
