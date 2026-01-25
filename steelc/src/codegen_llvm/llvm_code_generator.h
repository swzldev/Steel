#pragma once

#include <memory>
#include <unordered_map>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include <codegen/icode_generator.h>
#include <codegen/codegen_result.h>
#include <codegen/codegen_info.h>
#include <codegen/codegen_config.h>
#include <codegen/naming/name_mangler.h>
#include <codegen/sys/system_formats.h>
#include <codegen_llvm/type_handling/llvm_type_converter.h>
#include <codegen_llvm/builders/llvm_function_builder.h>
#include <codegen_llvm/builders/llvm_expression_builder.h>
#include <codegen_llvm/writers/llvm_writer.h>
#include <codegen_llvm/writers/llvm_native_writer.h>

// llvm_code_generator
//
// this is the built in and default LLVM based code generator for steelc
//
// codegen steps: mir -> llvm IR (bitcode/asm) -> native object

namespace llvm {
	class Function;
}

struct ssa_scope {
	std::unordered_map<mir_value::id_type, llvm::Value*> values;

	inline llvm::Value* get(mir_value::id_type id) {
		auto it = values.find(id);
		return it != values.end() ? it->second : nullptr;
	}

	inline void set(mir_value::id_type id, llvm::Value* val) {
		values[id] = val;
	}
};

class llvm_code_generator : public icode_generator {
private:
	static constexpr auto BITCODE_FORMAT = "llvm-bc";
	static constexpr auto ASSEMBLY_FORMAT = "llvm-asm";

public:
	llvm_code_generator()
		: builder(context),
		ty_converter(context),
		fn_builder(ty_converter),
		expression_builder(builder) {
	}

	codegen_info info() override {
		return {
			.name = "LLVM Code Generator",
			.description = "Generates LLVM IR and native objects using the LLVM framework",

			.capabilities = {
				codegen_capability::CAN_GENERATE_IR,
				codegen_capability::CAN_GENERATE_NATIVE_OBJECT
			},

			.supported_architectures = {
				platform_arch::X86,
				platform_arch::X64,
				platform_arch::ARM,
				platform_arch::ARM64
			},
			.supported_oses = {
				platform_os::LINUX,
				platform_os::WINDOWS,
				platform_os::OSX
			},
			.supported_abis = {
				platform_abi::GNU,
				platform_abi::MSVC,
				platform_abi::APPLE,
			},

			.supported_ir_formats = {
				BITCODE_FORMAT, // default
				ASSEMBLY_FORMAT
			},
		};
	}

	codegen_result emit(const mir_module& mod_mir, const codegen_config& cfg) override;

private:
	llvm::LLVMContext context;
	std::unique_ptr<llvm::Module> module = nullptr;
	llvm::IRBuilder<> builder;

	llvm::Function* current_func = nullptr;

	ssa_scope current_ssa;
	name_mangler mangler;
	llvm_type_converter ty_converter;
	llvm_function_builder fn_builder;
	llvm_expression_builder expression_builder;
	std::unique_ptr<llvm_writer> writer = nullptr;
	std::unique_ptr<llvm_native_writer> nwriter = nullptr;

	llvm::Function* emit_function(const mir_function& fn_mir);
	llvm::BasicBlock* emit_block(const mir_block& block_mir);
	void emit_instr(const mir_instr& instr_mir);

	llvm::Value* lower_operand(const mir_operand& op_mir);

	// artifact generators
	code_artifact generate_bitcode_artifact(const mir_module& mod_mir);
	code_artifact generate_asm_artifact(const mir_module& mod_mir);
	bool generate_native_object_artifact(const mir_module& mod_mir, code_artifact& out);

	template<typename T>
	struct always_false : std::false_type {};
};