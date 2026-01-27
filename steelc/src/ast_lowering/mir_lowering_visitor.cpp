#include "mir_lowering_visitor.h"

#include <memory>

#include <ast/ast.h>
#include <mir/mir_block.h>
#include <mir/mir_instr.h>
#include <mir/mir_function.h>
#include <mir/mir_type.h>
#include <mir/mir_operand.h>
#include <representations/entities/entity.h>
#include <representations/entities/module_entity.h>

void mir_lowering_visitor::visit(std::shared_ptr<variable_declaration> var) {
	// create a local mir_value for the variable
	mir_operand value{};

	if (var->has_initializer()) {
		// use initializer value
		value = accept(var->initializer);
	}
	else {
		// default value
		value = current_func.make_value({ var->type }, var->identifier);
	}

	// store it in the current scope
	locals_stack.back()[var->identifier] = value;
}
void mir_lowering_visitor::visit(std::shared_ptr<function_declaration> func) {
	// set the builder to the function's entry block
	if (current_func.blocks.empty()) {
		current_func.blocks.push_back(mir_block{ "entry" });
	}
	builder.set_function(&current_func);
	builder.set_insert_block(current_func.get_entry_block());

	push_scope();

	for (auto& param : current_func.params) {
		// create local for each parameter
		locals_stack.back()[param.name] = param.value;
	}

	// lower each statement in the function body
	func->body->accept(*this);
	pop_scope();
}
void mir_lowering_visitor::visit(std::shared_ptr<binary_expression> expr) {
	// lower left and right operands
	mir_operand left_op = accept(expr->left);
	mir_operand right_op = accept(expr->right);

	// build the binary instruction
	mir_operand bin_result = builder.build_binary_op(
		get_operator_opcode(expr->oparator),
		left_op,
		right_op
	);
	result = bin_result;
}
void mir_lowering_visitor::visit(std::shared_ptr<assignment_expression> expr) {
	mir_operand value = accept(expr->right);

	// for now, only support identifier expressions on the left side
	if (auto id_expr = std::dynamic_pointer_cast<identifier_expression>(expr->left)) {
		update_local(id_expr->identifier, value);
		result = value;
	}
	else {
		throw std::runtime_error("Unsupported left-hand side in assignment expression in MIR lowering");
	}
}
void mir_lowering_visitor::visit(std::shared_ptr<member_expression> expr) {
	if (expr->is_static_access()) {
		// static accesses purely semantic for now
		return;
	}

	mir_operand obj = accept(expr->object);
}
void mir_lowering_visitor::visit(std::shared_ptr<identifier_expression> expr) {
	if (expr->entity()->kind() == ENTITY_VARIABLE) {
		// local variable
		mir_operand local_op = get_local(expr->identifier);
		result = local_op;
	}
	else {
		throw std::runtime_error("Unsupported identifier entity kind in MIR lowering");
	}
}
void mir_lowering_visitor::visit(std::shared_ptr<cast_expression> expr) {
	// lower the inner expression
	mir_operand inner_op = accept(expr->expr);
	// build the cast instruction
	mir_operand cast_result = builder.build_cast(
		inner_op,
		mir_type{ expr->cast_type }
	);
	result = cast_result;
}
void mir_lowering_visitor::visit(std::shared_ptr<function_call> func_call) {
	// lower each argument
	std::vector<mir_operand> arg_operands;
	arg_operands.reserve(func_call->args.size());
	for (auto& arg_expr : func_call->args) {
		mir_operand arg_op = accept(arg_expr);
		arg_operands.push_back(arg_op);
	}

	std::vector<mir_type> generic_arg_types;
	generic_arg_types.reserve(func_call->generic_args.size());
	for (auto& gen_arg : func_call->generic_args) {
		generic_arg_types.push_back(mir_type{ gen_arg });
	}

	// build the call instruction
	mir_operand call_result = builder.build_call(
		func_call->declaration->parent_module->name_path(),
		func_call->identifier,
		generic_arg_types,
		arg_operands,
		mir_type{ func_call->declaration->type() } // declaration type for function type not return type
	);
	result = call_result;
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
		result = builder.build_const_float(
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
	if (!block->is_body) {
		push_scope();
	}
	// TODO: handle alloc/dealloc if non-body block
	for (auto& stmt : block->body) {
		stmt->accept(*this);
	}
	if (!block->is_body) {
		pop_scope();
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

mir_operand mir_lowering_visitor::get_local(const std::string& name) {
	for (auto it = locals_stack.rbegin(); it != locals_stack.rend(); ++it) {
		auto& scope = *it;
		auto found = scope.find(name);
		if (found != scope.end()) {
			return found->second;
		}
	}
	throw std::runtime_error("Local variable '" + name + "' not found in any scope");
}
void mir_lowering_visitor::update_local(const std::string& name, mir_operand value) {
	for (auto it = locals_stack.rbegin(); it != locals_stack.rend(); ++it) {
		auto& scope = *it;
		auto found = scope.find(name);
		if (found != scope.end()) {
			found->second = value;
			return;
		}
	}
	throw std::runtime_error("Local variable '" + name + "' not found in any scope");
}

mir_instr_opcode mir_lowering_visitor::get_operator_opcode(token_type op) {
	switch (op) {
	case TT_EQUAL:
		return mir_instr_opcode::CMP_EQ;
	case TT_NOT_EQUAL:
		return mir_instr_opcode::CMP_NEQ;
	case TT_ADD:
		return mir_instr_opcode::ADD;
	case TT_SUBTRACT:
		return mir_instr_opcode::SUB;
	case TT_MULTIPLY:
		return mir_instr_opcode::MUL;
	case TT_DIVIDE:
		return mir_instr_opcode::DIV;
	case TT_MODULO:
		return mir_instr_opcode::MOD;
	case TT_AND:
		return mir_instr_opcode::AND;
	case TT_OR:
		return mir_instr_opcode::OR;
	case TT_NOT:
		return mir_instr_opcode::NOT;
	case TT_LESS:
		return mir_instr_opcode::CMP_LT;
	case TT_LESS_EQ:
		return mir_instr_opcode::CMP_LTE;
	case TT_GREATER:
		return mir_instr_opcode::CMP_GT;
	case TT_GREATER_EQ:
		return mir_instr_opcode::CMP_GTE;

	default:
		throw std::runtime_error("Unsupported binary operator in MIR lowering");
	}
}
