#include "mir_lowering_visitor.h"

#include <memory>

#include <ast/ast.h>
#include <mir/mir_block.h>
#include <mir/mir_instr.h>
#include <mir/mir_function.h>
#include <mir/mir_type.h>
#include <mir/mir_operand.h>

void mir_lowering_visitor::visit(std::shared_ptr<function_declaration> func) {
	// set the builder to the function's entry block
	if (current_func.blocks.empty()) {
		current_func.blocks.push_back(mir_block{ "entry" });
	}
	builder.set_function(&current_func);
	builder.set_insert_block(current_func.get_entry_block());

	// lower each statement in the function body
	func->body->accept(*this);
}
void mir_lowering_visitor::visit(std::shared_ptr<literal> literal) {
	auto ty = literal->type(); // note: always a primitive

	if (ty->is_bool()) {
		// check bool first as it counts as integral
		// yet the value is a true/false string
		// in the future, perhaps i will just store it
		// as a 0 or 1
		int64_t val_int = literal->value == "true" ? 1 : 0;
		result = builder.build_const_int(
			val_int,
			mir_type{ ty }
		);
	}
	else if (ty->is_integral()) {
		int64_t val_int = std::stoll(literal->value);
		result = builder.build_const_int(
			val_int,
			mir_type{ ty }
		);
	}
	else if (ty->is_floating_point()) {
		double val_float = std::stoll(literal->value);
		result = builder.build_const_int(
			val_float,
			mir_type{ ty }
		);
	}
	else if (ty->is_text()) {
		result = builder.build_const_string(
			literal->value,
			mir_type{ ty }
		);
	}
	else {
		throw std::runtime_error("Unsupported literal type in MIR lowering");
	}
}
void mir_lowering_visitor::visit(std::shared_ptr<code_block> block) {
	// TODO: handle alloc/dealloc if non-body block
	for (auto& stmt : block->body) {
		stmt->accept(*this);
	}
}
void mir_lowering_visitor::visit(std::shared_ptr<return_statement> ret_stmt) {
	// lower the return expression
	if (ret_stmt->returns_value()) {
		mir_operand ret_value = accept(ret_stmt->value);
		builder.build_ret(ret_value);
	}
	else {
		// void return
		builder.build_ret_void();
	}
}