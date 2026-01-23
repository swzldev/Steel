#include "llvm_code_generator.h"

#include <memory>
#include <variant>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include <codegen/codegen_result.h>
#include <codegen/codegen_config.h>
#include <codegen/error/codegen_exception.h>
#include <codegen/sys/system_formats.h>
#include <codegen_llvm/type_handling/llvm_type_converter.h>
#include <codegen_llvm/builders/llvm_function_builder.h>
#include <codegen_llvm/builders/llvm_expression_builder.h>
#include <codegen_llvm/writers/llvm_writer.h>
#include <codegen_llvm/writers/llvm_native_writer.h>
#include <mir/mir_module.h>
#include <mir/mir_function.h>
#include <mir/mir_block.h>
#include <mir/mir_instr.h>

codegen_result llvm_code_generator::emit(const mir_module& mod_mir, const codegen_config& cfg) {
	module = std::make_unique<llvm::Module>(mod_mir.name, context);
	writer = std::make_unique<llvm_writer>(module.get());
	nwriter = std::make_unique<llvm_native_writer>(module.get(), cfg.target_triple, cfg.cpu);

	for (const auto& fn : mod_mir.functions) {
		emit_function(fn);
	}

	codegen_result result;
	// IR files (optional, but we produce them anyway)
	if (cfg.ir_format == "llvm-asm") {
		result.artifacts.push_back(generate_asm_artifact(mod_mir));
	}
	else if (cfg.ir_format == "llvm-bc" || cfg.ir_format.empty()) {
		result.artifacts.push_back(generate_bitcode_artifact(mod_mir));
	}
	else {
		throw std::runtime_error("Emit called with invalid format!");
	}

	// native object file
	result.artifacts.push_back(generate_native_object_artifact(mod_mir));

	return result;
}

llvm::Function* llvm_code_generator::emit_function(const mir_function& fn_mir) {
	current_ssa.values.clear(); // reset SSA values
	llvm::Function* fn_llvm = fn_builder.build(fn_mir, module.get());
	current_func = fn_llvm;

	//env = std::make_unique<codegen_env>(*module, *fn_llvm, builder);

	// map parameter values -> llvm values
	unsigned param_index = 0;
	for (const auto& param : fn_mir.params) {
		auto llvm_arg = fn_llvm->getArg(param_index++);
		current_ssa.set(param.value.get_id(), llvm_arg);
	}

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

	llvm::Value* result = nullptr;
	mir_value mir_result;

	switch (instr_mir.kind) {
	// NOP
	case mir_instr_opcode::NOP: {
		break;
	}

	// ARITHMETIC
	case mir_instr_opcode::ADD: {
		auto* lhs = lower_operand(instr_mir.operands[0]);
		auto* rhs = lower_operand(instr_mir.operands[1]);
		result = expression_builder.build_binary_expr(
			lhs,
			rhs,
			llvm_bin_op::ADD
		);
		break;
	}
	case mir_instr_opcode::SUB: {
		auto* lhs = lower_operand(instr_mir.operands[0]);
		auto* rhs = lower_operand(instr_mir.operands[1]);
		result = expression_builder.build_binary_expr(
			lhs,
			rhs,
			llvm_bin_op::SUB
		);
		break;
	}
	case mir_instr_opcode::MUL: {
		auto* lhs = lower_operand(instr_mir.operands[0]);
		auto* rhs = lower_operand(instr_mir.operands[1]);
		result = expression_builder.build_binary_expr(
			lhs,
			rhs,
			llvm_bin_op::MUL
		);
		break;
	}
	case mir_instr_opcode::DIV: {
		auto* lhs = lower_operand(instr_mir.operands[0]);
		auto* rhs = lower_operand(instr_mir.operands[1]);
		result = expression_builder.build_binary_expr(
			lhs,
			rhs,
			llvm_bin_op::DIV
		);
		break;
	}
	case mir_instr_opcode::MOD: {
		auto* lhs = lower_operand(instr_mir.operands[0]);
		auto* rhs = lower_operand(instr_mir.operands[1]);
		result = expression_builder.build_binary_expr(
			lhs,
			rhs,
			llvm_bin_op::REM
		);
		break;
	}

	// COMPARISON
	case mir_instr_opcode::CMP_EQ: {
		auto* lhs = lower_operand(instr_mir.operands[0]);
		auto* rhs = lower_operand(instr_mir.operands[1]);
		result = expression_builder.build_binary_expr(
			lhs,
			rhs,
			llvm_bin_op::EQUAL
		);
		break;
	}
	case mir_instr_opcode::CMP_NEQ: {
		auto* lhs = lower_operand(instr_mir.operands[0]);
		auto* rhs = lower_operand(instr_mir.operands[1]);
		result = expression_builder.build_binary_expr(
			lhs,
			rhs,
			llvm_bin_op::NOT_EQUAL
		);
		break;
	}
	case mir_instr_opcode::CMP_LT: {
		auto* lhs = lower_operand(instr_mir.operands[0]);
		auto* rhs = lower_operand(instr_mir.operands[1]);
		result = expression_builder.build_binary_expr(
			lhs,
			rhs,
			llvm_bin_op::LESS
		);
		break;
	}
	case mir_instr_opcode::CMP_LTE: {
		auto* lhs = lower_operand(instr_mir.operands[0]);
		auto* rhs = lower_operand(instr_mir.operands[1]);
		result = expression_builder.build_binary_expr(
			lhs,
			rhs,
			llvm_bin_op::LESS_EQ
		);
		break;
	}
	case mir_instr_opcode::CMP_GT: {
		auto* lhs = lower_operand(instr_mir.operands[0]);
		auto* rhs = lower_operand(instr_mir.operands[1]);
		result = expression_builder.build_binary_expr(
			lhs,
			rhs,
			llvm_bin_op::GREATER
		);
		break;
	}
	case mir_instr_opcode::CMP_GTE: {
		auto* lhs = lower_operand(instr_mir.operands[0]);
		auto* rhs = lower_operand(instr_mir.operands[1]);
		result = expression_builder.build_binary_expr(
			lhs,
			rhs,
			llvm_bin_op::GREATER_EQ
		);
		break;
	}
	

	// MEMORY OPERATIONS
	case mir_instr_opcode::LOAD: {
		// TODO
		break;
	}
	case mir_instr_opcode::STORE: {
		// TODO
		break;
	}

	// POINTER OPERATIONS
	case mir_instr_opcode::ADDR_OF: {
		// TODO
		break;
	}
	case mir_instr_opcode::DEREF: {
		// TODO
		break;
	}
	
	// DATA ACCESS AND CONVERSION
	case mir_instr_opcode::INDEX: {
		// TODO
		break;
	}
	case mir_instr_opcode::FIELD: {
		// TODO
		break;
	}
	case mir_instr_opcode::CAST: {
		// TODO
		break;
	}

	// FUNCTION CALL
	case mir_instr_opcode::CALL: {
		if (instr_mir.operands.empty()) {
			throw codegen_exception("CALL instruction with no operands, operands[0] should be function to call");
		}

		llvm::Value* callee = lower_operand(instr_mir.operands[0]);

		std::vector<llvm::Value*> args;
		args.reserve(instr_mir.operands.size() - 1);
		for (size_t i = 1; i < instr_mir.operands.size(); ++i) {
			args.push_back(lower_operand(instr_mir.operands[i]));
		}

		llvm::Function* fn = llvm::dyn_cast<llvm::Function>(callee);
		auto* fn_ty = fn->getFunctionType();

		cg_assert(fn_ty->getNumParams() == args.size(),
			"Function call argument count does not match function signature");

		result = builder.CreateCall(fn_ty, fn, args);
		break;
	}

	// RETURN FROM FUNCTION
	case mir_instr_opcode::RET: {
		if (instr_mir.operands.empty()) {
			// void return
			builder.CreateRetVoid();
		}
		else {
			llvm::Value* ret_val = lower_operand(instr_mir.operands[0]);
			builder.CreateRet(ret_val);
		}

		break;
	}

	default:
		throw codegen_exception("Unimplemented MIR instruction opcode in LLVM code generator");
		break;
	}

	// since mir_value is already identical to how llvm::Value would work,
	// we can entirely rely on the ssa_scope to map mir_value ids to llvm::Value pointers

	if (instr_mir.result.valid()) {
		if (result == nullptr) {
			throw codegen_exception("MIR instruction with result but no generated LLVM value");
		}
		current_ssa.set(instr_mir.result.get_id(), result);
	}
}

llvm::Value* llvm_code_generator::lower_operand(const mir_operand& op_mir) {
	return std::visit([this](auto&& arg) -> llvm::Value* {
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same_v<T, mir_value>) {
			llvm::Value* v = current_ssa.get(arg.get_id());
			return v;
		}
		else if constexpr (std::is_same_v<T, mir_const_int>) {
			llvm::Type* ty = ty_converter.convert(arg.type);
			return llvm::ConstantInt::get(
				ty,
				arg.value // TODO: handle signed vs unsigned
			);
		}
		else if constexpr (std::is_same_v<T, mir_const_float>) {
			llvm::Type* ty = ty_converter.convert(arg.type);
			return llvm::ConstantFP::get(
				ty,
				arg.value
			);
		}
		else if constexpr (std::is_same_v<T, mir_string_imm>) {
			return builder.CreateGlobalStringPtr(arg.value);
		}
		else if constexpr (std::is_same_v<T, mir_func_ref>) {
			std::string mangled = mangler.mangle_function(arg);
			if (llvm::Function* fn = module->getFunction(mangled)) {
				return fn;
			}
			// not defined in module - declare and return
			llvm::FunctionType* fn_type = llvm::cast<llvm::FunctionType>(ty_converter.convert(arg.type));
			llvm::Function* fn = llvm::Function::Create(
				fn_type,
				llvm::Function::ExternalLinkage,
				mangled,
				module.get()
			);
			return fn;
		}
		else if constexpr (std::is_same_v<T, mir_field_ref>) {
			// handle field reference
			// (not implemented yet)
			return nullptr;
		}
		else {
			static_assert(always_false<T>::value, "Non-exhaustive visitor in llvm_code_generator::lower_operand");
			return nullptr;
		}
	}, op_mir);
}

code_artifact llvm_code_generator::generate_bitcode_artifact(const mir_module& mod_mir) {
	return code_artifact{
		.kind = ARTIFACT_IR,
		.src_relpath = mod_mir.meta.src_relpath,
		.name = mod_mir.name,
		.extension = ".bc",
		.format = BITCODE_FORMAT,
		.is_binary = true,
		.bytes = writer->write_bitcode()
	};
}
code_artifact llvm_code_generator::generate_asm_artifact(const mir_module& mod_mir) {
	return code_artifact{
		.kind = ARTIFACT_IR,
		.src_relpath = mod_mir.meta.src_relpath,
		.name = mod_mir.name,
		.extension = ".ll",
		.format = ASSEMBLY_FORMAT,
		.is_binary = false,
		.text = writer->write_asm()
	};
}
code_artifact llvm_code_generator::generate_native_object_artifact(const mir_module& mod_mir) {
	std::string fmt = system_formats::get_object_format();
	std::string ext = system_formats::get_object_extension();
	return code_artifact{
		.kind = ARTIFACT_OBJECT,
		.src_relpath = mod_mir.meta.src_relpath,
		.name = mod_mir.name,
		.extension = ext,
		.format = fmt,
		.is_binary = true,
		.bytes = nwriter->write_object()
	};
}
