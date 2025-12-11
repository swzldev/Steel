#include "compile_time_computator.h"

#include <memory>

#include <lexer/token_type.h>
#include <ast_passes/classes/computed_value.h>
#include <ast/expressions/binary_expression.h>
#include <ast/expressions/literal.h>

void compile_time_computator_visitor::visit(std::shared_ptr<binary_expression> expr) {
	if (!expr->left->is_constant() || !expr->right->is_constant()) {
		return;
	}

	auto left_val = accept(expr->left);
	auto right_val = accept(expr->right);

	switch (expr->oparator) {
		case TT_EQUAL:
			result = computed_value(left_val == right_val);
			break;
		case TT_NOT_EQUAL:
			result = computed_value(left_val != right_val);
			break;
		case TT_ADD:
			result = computed_value(left_val + right_val);
			break;
		case TT_SUBTRACT:
			result = computed_value(left_val == right_val);
			break;
		case TT_MULTIPLY:
			result = computed_value(left_val == right_val);
			break;
		case TT_DIVIDE:
			result = computed_value(left_val == right_val);
			break;
		case TT_MODULO:
			result = computed_value(left_val == right_val);
			break;
		case TT_AND:
			result = computed_value(left_val == right_val);
			break;
		case TT_OR:
			result = computed_value(left_val == right_val);
			break;
		case TT_NOT:
			result = computed_value(left_val == right_val);
			break;
		case TT_LESS:
			result = computed_value(left_val == right_val);
			break;
		case TT_LESS_EQ:
			result = computed_value(left_val == right_val);
			break;
		case TT_GREATER:
			result = computed_value(left_val == right_val);
			break;
		case TT_GREATER_EQ:
			result = computed_value(left_val == right_val);
			break;
	}
}
void compile_time_computator_visitor::visit(std::shared_ptr<literal> literal) {
	result = computed_value(literal->type(), literal->value);
}

void compile_time_computator_visitor::not_constant_expr() const {
	//throw std::runtime_error("Attempted to compute a non-constant expression at compile time.");
}
