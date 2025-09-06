#include "flow_analyzer.h"

#include "../ast/ast.h"

void flow_analyzer::visit(std::shared_ptr<function_declaration> func) {
	current_function = func;
	current_returns = false;
	func->body->accept(*this);
	current_function = nullptr;

	// check if we dont return on all code paths, and the function is not void
	if (!current_returns && func->return_type->is_primitive() && func->return_type->primitive != DT_VOID) {
		ERROR(ERR_NOT_ALL_PATHS_RETURN_VALUE, func->position);
	}
}
void flow_analyzer::visit(std::shared_ptr<constructor_declaration> constructor) {
	current_constructor = constructor;
	current_returns = false;
	constructor->body->accept(*this);
	current_constructor = nullptr;
}
void flow_analyzer::visit(std::shared_ptr<constructor_call> ctor_call) {
	// if the constructor call declaration is the same as we are in,
	// it would result in a stack overflow
	if (current_constructor && ctor_call->declaration == current_constructor) {
		ERROR(ERR_CONSTRUCTOR_CALLS_ITSELF, ctor_call->position);
		return;
	}
}
void flow_analyzer::visit(std::shared_ptr<block_statement> block) {
	size_t i = 0;
	for (; i < block->body.size(); i++) {
		block->body[i]->accept(*this);
		if (current_returns) {
			++i;
			break;
		}
	}
	for (; i < block->body.size(); i++) {
		// if there are more statements, unreachable code
		WARN(WARN_UNREACHABLE_CODE, block->body[i]->position);
	}
}
void flow_analyzer::visit(std::shared_ptr<return_statement> ret_stmt) {
	if (!current_function && !current_constructor) {
		ERROR(ERR_RETURN_OUTSIDE_FUNCTION, ret_stmt->position);
		return;
	}

	if (current_constructor && ret_stmt->returns_value()) {
		ERROR(ERR_CONSTRUCTOR_RETURNS_VALUE, ret_stmt->position);
		return;
	}

	auto& func_type = current_function->return_type;
	if (ret_stmt->value) {
		// check if function is void
		if (func_type->is_primitive() && func_type->primitive == DT_VOID) {
			ERROR(ERR_VOID_FUNCTION_RETURNS_VALUE, ret_stmt->position, current_function->identifier.c_str());
			return;
		}

		ret_stmt->value->accept(*this);
		// ensure types match
		if (!ret_stmt->value->type()) {
			ERROR(ERR_FUNCTION_RETURN_TYPE_MISMATCH, ret_stmt->position, current_function->identifier.c_str(), func_type->type_name().c_str(), "<Unknown Type>");
			return;
		}
		else if (*func_type != *ret_stmt->value->type()) {
			ERROR(ERR_FUNCTION_RETURN_TYPE_MISMATCH, ret_stmt->position, current_function->identifier.c_str(), func_type->type_name().c_str(), ret_stmt->value->type()->type_name().c_str());
			return;
		}

		current_returns = true; // function returns a value
	}
	else {
		// ensure function is void
		if (!func_type->is_primitive() || func_type->primitive != DT_VOID) {
			ERROR(ERR_FUNCTION_MUST_RETURN_VALUE, ret_stmt->position, current_function->identifier.c_str());
			return;
		}

		current_returns = true; // function returns void
	}
}
void flow_analyzer::visit(std::shared_ptr<return_if> ret_stmt) {
	if (!current_function && !current_constructor) {
		ERROR(ERR_RETURN_OUTSIDE_FUNCTION, ret_stmt->position);
		return;
	}

	if (current_constructor && ret_stmt->returns_value()) {
		ERROR(ERR_CONSTRUCTOR_RETURNS_VALUE, ret_stmt->position);
		return;
	}

	auto& func_type = current_function->return_type;
	if (ret_stmt->value) {
		// check if function is void
		if (func_type->is_primitive() && func_type->primitive == DT_VOID) {
			ERROR(ERR_VOID_FUNCTION_RETURNS_VALUE, ret_stmt->position, current_function->identifier.c_str());
			return;
		}

		ret_stmt->value->accept(*this);
		// ensure types match
		if (!ret_stmt->value->type()) {
			ERROR(ERR_FUNCTION_RETURN_TYPE_MISMATCH, ret_stmt->position, current_function->identifier.c_str(), func_type->type_name().c_str(), "<Unknown Type>");
			return;
		}
		else if (*func_type != *ret_stmt->value->type()) {
			ERROR(ERR_FUNCTION_RETURN_TYPE_MISMATCH, ret_stmt->position, current_function->identifier.c_str(), func_type->type_name().c_str(), ret_stmt->value->type()->type_name().c_str());
			return;
		}

		current_returns = false; // function returns a value
	}
	else {
		// ensure function is void
		if (!func_type->is_primitive() || func_type->primitive != DT_VOID) {
			ERROR(ERR_FUNCTION_MUST_RETURN_VALUE, ret_stmt->position, current_function->identifier.c_str());
			return;
		}

		current_returns = false; // function returns void
	}
}
void flow_analyzer::visit(std::shared_ptr<if_statement> if_stmt) {
	// check for an always true condition
	if (auto lit = std::dynamic_pointer_cast<literal>(if_stmt->condition)) {
		// TODO
	}
	if_stmt->condition->accept(*this);

	bool then_returns = false, else_returns = false;
	if_stmt->then_block->accept(*this);
	then_returns = current_returns;
	current_returns = false;

	if (if_stmt->else_block) {
		// could be a block statement or another if (for an else if)
		if_stmt->else_block->accept(*this);
		else_returns = current_returns;
		current_returns = false;
	}

	// if both branches return, the if statement returns
	if (then_returns && else_returns) {
		current_returns = true;
	}
	else if (then_returns || else_returns) {
		current_returns = false; // only one branch returns so we cannot be sure
	}
	else {
		current_returns = false; // no returns in either branch
	}
}
//void flow_analyzer::visit(std::shared_ptr<for_loop> for_loop) {
//}
//void flow_analyzer::visit(std::shared_ptr<while_loop> while_loop) {
//}