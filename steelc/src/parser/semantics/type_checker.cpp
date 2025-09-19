#include "type_checker.h"

#include <memory>

#include "../ast/ast.h"
#include "../types/custom_types.h"
#include "../types/core.h"
#include "../types/type_utils.h"

void type_checker::visit(std::shared_ptr<variable_declaration> var) {
	// accept initializer early to prevent double visits
	if (var->has_initializer()) {
		var->initializer->accept(*this);
	}

	// set variable type if unknown
	if (var->type == data_type::unknown) {
		if (var->has_initializer()) {
			var->type = var->initializer->type();
		}
		else {
			ERROR(ERR_CANNOT_INFER_TYPE_NO_INIT, var->position, var->identifier.c_str());
			return;
		}
	}
	else {
		// if variable type is custom, ensure it is defined
		if (auto custom = std::dynamic_pointer_cast<custom_type>(var->type)) {
			if (!custom->declaration) {
				ERROR(ERR_TYPE_NOT_DEFINED, var->position, custom->name().c_str());
				return;
			}
		}
	}
	// ensure initializer is valid
	if (var->has_initializer()) {
		if (auto init_list = std::dynamic_pointer_cast<initializer_list>(var->initializer)) {
			if (var->type->is_array()) {
				// array
				ERROR(ERR_ARRAY_INITIALIZER_UNSUPPORTED, var->position);
				return;
			}
			else if (auto custom = std::dynamic_pointer_cast<custom_type>(var->type)) {
				// struct/class
				if (custom->declaration->type_kind == CT_STRUCT || custom->declaration->type_kind == CT_CLASS) {
					if (init_list->values.size() > custom->declaration->fields.size()) {
						ERROR(ERR_TOO_MANY_INITIALIZER_VALUES, init_list->position);
						return;
					}
					else {
						for (int i = 0; i < init_list->values.size(); i++) {
							if (*custom->declaration->fields[i]->type != *init_list->values[i]->type()) {
								ERROR(ERR_TYPE_MISMATCH_INITIALIZER, init_list->position);
								return;
							}
						}
					}
				}
				// interface
				else {
					ERROR(ERR_INTERFACE_INITIALIZER, var->position);
					return;
				}
			}
			else {
				ERROR(ERR_INVALID_INITIALIZER_LIST_USAGE, var->position);
				return;
			}
		}
		// standard variable assignment
		else {
			auto& var_type = var->type;
			auto init_type = var->initializer->type();
			if (*var_type != *init_type && !is_valid_conversion(init_type, var_type, true, var->initializer->position)) {
				ERROR(ERR_TYPE_ASSIGNMENT_MISMATCH, var->position, var->type->name().c_str(), var->initializer->type()->name().c_str());
				return;
			}
		}
	}
}
void type_checker::visit(std::shared_ptr<type_declaration> decl) {
	std::unordered_map<std::shared_ptr<function_declaration>, bool> interface_funcs;
	if (decl->type_kind == CT_CLASS) {
		bool first = true;
		bool derives_class = false;
		for (const auto& base : decl->base_types) {
			if (auto custom = std::dynamic_pointer_cast<custom_type>(base)) {
				if (custom->declaration->type_kind == CT_CLASS) {
					if (derives_class) {
						ERROR(ERR_MULTIPLE_BASE_CLASSES, decl->position);
						return;
					}
					if (!first) {
						ERROR(ERR_BASE_CLASS_NOT_FIRST, decl->position, base->name());
						return;
					}
					decl->base_class = custom->declaration;
					derives_class = true;
				}
				else if (custom->declaration->type_kind == CT_INTERFACE) {
					// add all interface methods to map
					for (const auto& method : custom->declaration->methods) {
						interface_funcs[method] = false;
					}
				}
			}
			if (first) first = false;
		}
	}

	for (const auto& constructor : decl->constructors) {
		constructor->accept(*this);
	}
	for (const auto& member : decl->fields) {
		member->accept(*this);
	}
	for (const auto& method : decl->methods) {
		// accept method as normal
		method->accept(*this);

		// methods in interfaces should have no body
		if (decl->type_kind == CT_INTERFACE && method->body) {
			ERROR(ERR_INTERFACE_METHOD_HAS_BODY, method->position);
			return;
		}

		if (method->is_override) {
			bool found = false;
			// check if it matches an interface function
			for (auto& [iface_method, implemented] : interface_funcs) {
				if (method->matches(iface_method, false)) {
					implemented = true;
					method->overridden_function = iface_method;
					// ensure return type of method is set to match the interface
					method->return_type = iface_method->return_type;
					found = true;
					break;
				}
			}
			if (!found) {
				ERROR(ERR_OVERRIDE_NOT_FOUND, method->position, method->identifier.c_str());
				return;
			}
		}
	}
	for (const auto& op : decl->operators) {
		op->accept(*this);
	}

	// ensure all interface methods are implemented
	if (decl->type_kind == CT_CLASS) {
		for (const auto& iface_method : interface_funcs) {
			if (!iface_method.second) {
				ERROR(ERR_INTERFACE_METHOD_NOT_IMPLEMENTED, decl->position, decl->identifier.c_str(), iface_method.first->identifier.c_str());
				return;
			}
		}
	}
}
void type_checker::visit(std::shared_ptr<binary_expression> expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
	auto left_type = expr->left->type();
	auto right_type = expr->right->type();

	const auto& builtin_operators = get_core_operators();
	// if both sides are primitive types, check if a built in operator exists
	if (left_type->is_primitive() && right_type->is_primitive()) {
		for (const auto& op : builtin_operators) {
			if (op.matches(left_type, expr->oparator, right_type)) {
				expr->result_type = op.result_type;
				return;
			}
		}
		ERROR(ERR_NO_MATCHING_OPERATOR, expr->position);
		return;
	}
	else {
		// if at least one side is custom, we can check user-defined operators
		if (auto left_custom = std::dynamic_pointer_cast<custom_type>(left_type)) {
			if (!left_custom->declaration) {
				ERROR(ERR_TYPE_NOT_DEFINED, expr->position, left_custom->name().c_str());
				return;
			}
			for (const auto& op : left_custom->declaration->operators) {
				if (op->matches(left_type, expr->oparator, right_type)) {
					expr->result_type = op->result_type;
					return;
				}
			}
		}
		if (auto right_custom = std::dynamic_pointer_cast<custom_type>(right_type)) {
			if (!right_custom->declaration) {
				ERROR(ERR_TYPE_NOT_DEFINED, expr->position, right_custom->name().c_str());
				return;
			}
			for (const auto& op : right_custom->declaration->operators) {
				if (op->matches(left_type, expr->oparator, right_type)) {
					expr->result_type = op->result_type;
					return;
				}
			}
		}
	}
	// no built-in or user-defined operators available
	ERROR(ERR_NO_MATCHING_OPERATOR_BUILTIN_USER, expr->position);
}
void type_checker::visit(std::shared_ptr<assignment_expression> expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
	auto left_type = expr->left->type();
	auto right_type = expr->right->type();

	// cannot assign to a const variable
	if (auto left_var = std::dynamic_pointer_cast<identifier_expression>(expr->left)) {
		if (left_var->declaration->is_const) {
			ERROR(ERR_CONST_ASSIGNMENT, expr->position, left_var->declaration->identifier.c_str());
			return;
		}
	}

	// cannot assign to an rvalue
	if (expr->left->is_rvalue()) {
		ERROR(ERR_ASSIGNMENT_TO_RVALUE, expr->position);
		return;
	}

	// mismatch assignment
	if (*left_type != *right_type && !is_valid_conversion(right_type, left_type, true, expr->right->position)) {
		ERROR(ERR_TYPE_ASSIGNMENT_MISMATCH, expr->position, left_type->name().c_str(), right_type->name().c_str());
		return;
	}
}
void type_checker::visit(std::shared_ptr<address_of_expression> expr) {
	expr->value->accept(*this);
	if (!std::dynamic_pointer_cast<identifier_expression>(expr->value)) {
		ERROR(ERR_ADDRESS_OF_RVALUE, expr->position);
		return;
	}
}
void type_checker::visit(std::shared_ptr<unary_expression> expr) {
	expr->operand->accept(*this);
	auto operand_type = expr->operand->type();
	switch (expr->oparator) {
	case TT_NOT:
		if (!operand_type->is_primitive() || operand_type->primitive != DT_BOOL) {
			ERROR(ERR_LOGICAL_NOT_BOOLEAN_ONLY, expr->position);
			return;
		}
		break;
	case TT_SUBTRACT:
		if (!operand_type->is_primitive() || operand_type->primitive != DT_I32 && operand_type->primitive != DT_FLOAT) {
			ERROR(ERR_NEGATE_NUMERIC_ONLY, expr->position);
			return;
		}
		break;
	case TT_INCREMENT:
		if (!operand_type->is_primitive() || operand_type->primitive != DT_I32) {
			ERROR(ERR_INCREMENT_INTEGER_ONLY, expr->position);
			return;
		}
		break;
	case TT_DECREMENT:
		if (!operand_type->is_primitive() || operand_type->primitive != DT_I32) {
			ERROR(ERR_DECREMENT_INTEGER_ONLY, expr->position);
			return;
		}
		break;
	}
}
void type_checker::visit(std::shared_ptr<index_expression> expr) {
	expr->base->accept(*this);
	if (!expr->base->type()->is_indexable()) {
		ERROR(ERR_BASE_NOT_INDEXABLE, expr->position);
		return;
	}
	expr->indexer->accept(*this);
	if (!expr->indexer->type()->is_primitive() || expr->indexer->type()->primitive != DT_I32) {
		ERROR(ERR_INDEXER_NOT_INTEGER, expr->indexer->position);
		return;
	}
}
void type_checker::visit(std::shared_ptr<cast_expression> expr) {
	expr->expression->accept(*this);

	auto from = expr->expression->type();
	auto& to = expr->cast_type;

	if (!is_valid_conversion(from, to, false, expr->position)) {
		ERROR(ERR_NO_CONVERSION_EXISTS, expr->position, from->name().c_str(), to->name().c_str());
		return;
	}
}
void type_checker::visit(std::shared_ptr<member_expression> expr) {
	expr->object->accept(*this);
	auto type = expr->object->type();
	if (type == data_type::unknown) {
		// object resolution error, ignore for simplicity
		return;
	}

	auto custom = std::dynamic_pointer_cast<custom_type>(type);
	if (!custom || !custom->declaration) {
		ERROR(ERR_MEMBER_ACCESS_ON_NONCOMPOSITE, expr->position, type->name().c_str());
		return;
	}

	auto& type_decl = custom->declaration;
	bool found = false;
	do {
		for (const auto& member : custom->declaration->fields) {
			if (member->identifier == expr->member) {
				expr->declaration = member;
				found = true;
				break;
			}
		}
		type_decl = type_decl->base_class;
	} while (type_decl != nullptr);

	if (!found) {
		ERROR(ERR_NO_MEMBER_WITH_NAME, expr->position, custom->name().c_str(), expr->member.c_str());
		return;
	}
}
void type_checker::visit(std::shared_ptr<function_call> func_call) {
	std::vector<type_ptr> arg_types;
	for (const auto& arg : func_call->args) {
		arg->accept(*this);
		arg_types.push_back(arg->type());
	}

	// we need to resolve candidates here if the function call
	// is a method, as it cant be done in the name resolver pass
	if (func_call->is_method()) {
		if (auto member = std::dynamic_pointer_cast<member_expression>(func_call->callee)) {
			// resolve the object
			member->object->accept(*this);

			auto type = member->object->type();
			if (type == data_type::unknown) {
				// object resolution error, ignore for simplicity
				return;
			}

			auto custom = std::dynamic_pointer_cast<custom_type>(type);
			if (!custom || !custom->declaration) {
				ERROR(ERR_MEMBER_ACCESS_ON_NONCOMPOSITE, func_call->position, type->name().c_str());
				return;
			}

			// find method - dont check for return type as its unknown in a call
			auto method_candidates = get_method_candidates(custom->declaration, func_call->identifier, func_call->args.size());
			func_call->declaration_candidates = method_candidates;
		}
		else {
			// maybe we throw an error? cant remember if this is a possible case
		}
	}

	// TEMPORARY
	if (func_call->identifier == "Print") return;

	// check any candidates match arguments provided
	bool matches = false;
	for (const auto& candidate : func_call->declaration_candidates) {
		if (candidate->is_generic) {
			// we need to substitute generic types here

		}

		auto expected_types = candidate->get_expected_types();
		if (expected_types.size() != arg_types.size()) {
			ERROR(ERR_INTERNAL_ERROR, func_call->position, "Type Checker", "Argument count for candidate doesnt match");
			continue; // argument count doesn't match
		}

		// check all args match for each candidate
		// there should be at least one perfect match otherwise
		// we give an error
		// note currently there is no support for implicit conversions here
		bool all_match = true;
		for (size_t i = 0; i < expected_types.size(); ++i) {
			if (*expected_types[i] != *arg_types[i]) {
				all_match = false;
				break;
			}
		}
		if (all_match) {
			func_call->declaration = candidate;
			matches = true;
			break;
		}
	}
	if (!matches) {
		if (func_call->is_constructor()) {
			ERROR(ERR_NO_MATCHING_CONSTRUCTOR, func_call->position, func_call->identifier.c_str());
			return;
		}
		else if (func_call->is_method()) {
			ERROR(ERR_NO_MATCHING_METHOD, func_call->position, func_call->identifier.c_str());
		}
		else {
			ERROR(ERR_NO_MATCHING_FUNCTION, func_call->position, func_call->identifier.c_str());
		}
		return;
	}
}
void type_checker::visit(std::shared_ptr<if_statement> if_stmt) {
	if_stmt->condition->accept(*this);
	if (!if_stmt->condition->type()->is_primitive() || if_stmt->condition->type()->primitive != DT_BOOL) {
		ERROR(ERR_IF_CONDITION_NOT_BOOLEAN, if_stmt->position);
		return;
	}
	if_stmt->then_block->accept(*this);
	if (if_stmt->else_block) {
		if_stmt->else_block->accept(*this);
	}
}
void type_checker::visit(std::shared_ptr<inline_if> inline_if) {
	inline_if->condition->accept(*this);
	if (!inline_if->condition->type()->is_primitive() || inline_if->condition->type()->primitive != DT_BOOL) {
		ERROR(ERR_IF_CONDITION_NOT_BOOLEAN, inline_if->position);
		return;
	}
	inline_if->statement->accept(*this);
}
void type_checker::visit(std::shared_ptr<for_loop> for_loop) {
	if (for_loop->initializer) {
		for_loop->initializer->accept(*this);
	}
	// condition should always be a boolean expression
	if (for_loop->condition) {
		for_loop->condition->accept(*this);
		if (!for_loop->condition->type()->is_primitive() || for_loop->condition->type()->primitive != DT_BOOL) {
			ERROR(ERR_FOR_CONDITION_NOT_BOOLEAN, for_loop->position);
			return;
		}
	}
	if (for_loop->increment) {
		for_loop->increment->accept(*this);
	}
}
void type_checker::visit(std::shared_ptr<while_loop> while_loop) {
	while_loop->condition->accept(*this);
	if (!while_loop->condition->type()->is_primitive() || while_loop->condition->type()->primitive != DT_BOOL) {
		ERROR(ERR_WHILE_CONDITION_NOT_BOOLEAN, while_loop->position);
		return;
	}
}

bool type_checker::is_valid_conversion(type_ptr from, type_ptr to, bool implicit, position pos) {
	const auto& builtin_conversions = get_core_conversions(from->primitive);
	for (const auto& conv : builtin_conversions) {
		if (*conv.to == *to) {
			if (!implicit && !conv.implicit) {
				return true;
			}
			else if (!implicit && conv.implicit) {
				WARN(WARN_CAST_UNNEEDED, pos, from->name().c_str(), to->name().c_str());
				return true;
			}
			else /* implicit && !conv.implicit */ {
				ERROR(ERR_CONVERSION_EXPLICIT_CAST_REQUIRED, pos, from->name().c_str(), to->name().c_str());
				return false;
			}
		}
	}
	return false;
}