#include "flow_analyzer.h"

#include "../ast/ast.h"

void flow_analyzer::visit(std::shared_ptr<function_declaration> func) {
	if (func->is_abstract()) {
		return;
	}

	if (func->is_constructor) {
		current_constructor = func;
	}
	else {
		current_function = func;
	}

	current_returns = false;
	func->body->accept(*this);

	current_function = nullptr;
	current_constructor = nullptr;
	

	// check if we dont return on all code paths, and the function is not void
	if (!current_returns && func->return_type->is_primitive() && func->return_type->primitive != DT_VOID) {
		ERROR(ERR_NOT_ALL_PATHS_RETURN_VALUE, func->position);
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

	current_returns = true;

	if (current_constructor && ret_stmt->returns_value()) {
		ERROR(ERR_CONSTRUCTOR_RETURNS_VALUE, ret_stmt->position);
		return;
	}
}
void flow_analyzer::visit(std::shared_ptr<return_if> ret_stmt) {
	if (!current_function && !current_constructor) {
		ERROR(ERR_RETURN_OUTSIDE_FUNCTION, ret_stmt->position);
		return;
	}

	current_returns = false; // may not return

	if (current_constructor && ret_stmt->returns_value()) {
		ERROR(ERR_CONSTRUCTOR_RETURNS_VALUE, ret_stmt->position);
		return;
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