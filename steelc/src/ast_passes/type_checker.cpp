#include "type_checker.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <string>

#include <ast/ast.h>
#include <ast_passes/generic_substitutor.h>
#include <lexer/token_type.h>
#include <lexer/token.h>
#include <parser/parser_utils.h>
#include <compiler/compilation_pass.h>
#include <steelc/language_constants.h>
#include <error/error_catalog.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/types/custom_type.h>
#include <representations/types/container_types.h>
#include <representations/types/enum_type.h>
#include <representations/types/core.h>
#include <representations/types/type_utils.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity.h>
#include <representations/entities/variable_entity.h>
#include <representations/entities/function_entity.h>
#include <symbolics/symbol_table.h>
#include <symbolics/overlay_table.h>

void type_checker::visit(std::shared_ptr<function_declaration> func) {
	if (func->is_generic && !func->is_generic_instance) {
		return;
	}

	check_type(func->return_type);
	for (const auto& param : func->parameters) {
		check_type(param->type);
	}

	if (is_valid_entry_point(func)) {
		// check for entry point overloads
		if (module_manager.entry_point) {
			ERROR(ERR_ENTRY_OVERLOADED, func->span, language_constants::ENTRY_POINT);
			return;
		}
		else {
			module_manager.entry_point = func;
			func->is_entry_point = true;
		}
	}
	else {
		// if its not a valid entry point, we can just ignore it
		// if no entry point is found an error will occur anyway
	}

	current_function = func;
	if (func->body) {
		func->body->accept(*this);
	}
	current_function = nullptr;
}
void type_checker::visit(std::shared_ptr<variable_declaration> var) {
	if (var->type != data_type::UNKNOWN) {
		// only try and unbox for explicitly typed variables
		check_type(var->type);
	}

	// accept initializer early to prevent double visits
	if (var->has_initializer()) {
		var->initializer->accept(*this);
	}

	// set variable type if UNKNOWN
	if (var->type == data_type::UNKNOWN) {
		if (var->has_initializer()) {
			auto init_type = var->initializer->type();
			if (init_type == data_type::UNKNOWN) {
				ERROR(ERR_CANNOT_INFER_TYPE_UNKNOWN_INIT, var->span, var->identifier.c_str());
				return;
			}
			var->type = var->initializer->type();
		}
		else {
			ERROR(ERR_CANNOT_INFER_TYPE_NO_INIT, var->span, var->identifier.c_str());
			return;
		}
	}
	else {
		// if variable type is custom, ensure it is defined
		if (auto custom = var->type->as_custom()) {
			if (!custom->declaration) {
				ERROR(ERR_TYPE_NOT_DEFINED, var->span, custom->name().c_str());
				return;
			}
		}
	}
	// ensure initializer is valid
	if (var->has_initializer()) {
		if (auto init_list = std::dynamic_pointer_cast<initializer_list>(var->initializer)) {
			if (init_list->is_array_initializer) {
				if (!var->type->as_array()) {
					ERROR(ERR_INVALID_ARRAY_INITIALIZER_USAGE, var->span);
					return;
				}
			}
			else if (auto custom = var->type->as_custom()) {
				// struct/class
				if (custom->declaration->type_kind == CT_STRUCT || custom->declaration->type_kind == CT_CLASS) {
					if (init_list->values.size() > custom->declaration->fields.size()) {
						ERROR(ERR_TOO_MANY_INITIALIZER_VALUES, init_list->span);
						return;
					}
					else {
						for (size_t i = 0; i < init_list->values.size(); i++) {
							const auto& value_type = init_list->values[i]->type();
							const auto& field_type = custom->declaration->fields[i]->type;
							if (*field_type != value_type) {
								ERROR(ERR_TYPE_MISMATCH_INITIALIZER, init_list->values[i]->span, field_type->name().c_str(), value_type->name().c_str());
								return;
							}
						}
					}
				}
				// interface
				else {
					ERROR(ERR_INTERFACE_INITIALIZER, var->span);
					return;
				}
				// set result type (assuming no errors)
				init_list->result_type = var->type;
			}
			else {
				ERROR(ERR_INVALID_INITIALIZER_LIST_USAGE, var->span);
				return;
			}
		}
		// standard variable assignment
		else {
			auto& var_type = var->type;
			auto init_type = var->initializer->type();
			if (init_type == data_type::UNKNOWN) {
				return; // might change this later
				ERROR(ERR_CANNOT_INFER_TYPE_UNKNOWN_INIT, var->span, var->identifier.c_str());
				return;
			}
			if (*var_type != init_type && !is_valid_conversion(init_type, var_type, true, var->initializer->span)) {
				ERROR(ERR_TYPE_ASSIGNMENT_MISMATCH, var->span, var->type->name().c_str(), var->initializer->type()->name().c_str());
				return;
			}
		}
	}
}
void type_checker::visit(std::shared_ptr<type_declaration> decl) {
	if (decl->is_generic && !decl->is_generic_instance) {
		return;
	}

	std::unordered_map<std::shared_ptr<function_declaration>, bool> interface_funcs;
	size_t implemented_interface_count = 0;
	if (decl->type_kind == CT_CLASS) {
		bool first = true;
		bool derives_class = false;
		for (const auto& base : decl->base_types) {
			if (auto custom = base->as_custom()) {
				if (custom->declaration->type_kind == CT_CLASS) {
					if (derives_class) {
						ERROR(ERR_MULTIPLE_BASE_CLASSES, decl->span);
						return;
					}
					if (!first) {
						ERROR(ERR_BASE_CLASS_NOT_FIRST, decl->span, base->name());
						return;
					}
					decl->base_type = custom->declaration;
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

		// check for a circular inheritance chain
		std::unordered_set<std::string> visited;
		std::string chain;
		std::shared_ptr<const type_declaration> current = decl;
		while (current) {
			chain += current->identifier;
			if (visited.count(current->identifier)) {
				ERROR(ERR_CIRCULAR_INHERITANCE, decl->span, decl->name().c_str(), chain.c_str());
				return;
			}
			if (current->base_type) chain += " -> ";
			visited.insert(current->identifier);
			current = current->base_type;
		}
	}

	for (const auto& constructor : decl->constructors) {
		constructor->accept(*this);
	}
	for (const auto& member : decl->fields) {
		member->accept(*this);
		// cannot contain fields of own type
		if (auto custom = member->type->as_custom()) {
			// should be okay, but i should note its generally
			// not safe to compare pointers like this
			if (custom->declaration == decl) {
				ERROR(ERR_FIELD_CANNOT_BE_OWN_TYPE, member->span, member->identifier.c_str(), decl->identifier.c_str());
				ADVISE(ADV_USE_POINTER_INSTEAD_OF_OWN_TYPE, member->span, decl->identifier.c_str());
				return;
			}
		}
	}
	for (const auto& method : decl->methods) {
		// check method
		if (method->is_override) {
			bool found = false;
			// check if it matches an interface function
			for (auto& [iface_method, implemented] : interface_funcs) {
				method->return_type = iface_method->return_type;
				if (method->identifier == iface_method->identifier) {
					for (int i = 0; i < method->parameters.size(); i++) {
						if (i >= iface_method->parameters.size() ||
							*method->parameters[i]->type != iface_method->parameters[i]->type) {
							continue;
						}
					}

					implemented = true;
					method->overridden_function = iface_method;
					found = true;
					break;
				}
			}
			// TODO: add support for overriding base class virtual methods
			if (!found) {
				method->return_type = nullptr;
				ERROR(ERR_OVERRIDE_NOT_FOUND, method->span, method->identifier.c_str());
				return;
			}
		}

		// accept method as normal
		method->accept(*this);

		// methods in interfaces should have no body
		if (decl->type_kind == CT_INTERFACE && method->body) {
			ERROR(ERR_INTERFACE_METHOD_HAS_BODY, method->span);
			return;
		}
	}
	for (const auto& op : decl->operators) {
		op->accept(*this);
	}

	// ensure all interface methods are implemented
	if (decl->type_kind == CT_CLASS) {
		if (implemented_interface_count < interface_funcs.size()) {
			ERROR(ERR_NOT_ALL_INTERFACE_METHODS_IMPLEMENTED, decl->span, decl->identifier.c_str(), implemented_interface_count, interface_funcs.size());
		}
		for (const auto& iface_method : interface_funcs) {
			if (!iface_method.second) {
				ADVISE(ADV_IMPLEMENT_INTERFACE_METHOD, decl->span, iface_method.first->identifier);
			}
		}
	}
}
void type_checker::visit(std::shared_ptr<module_declaration> module) {
	// mostly for updating the current symbol table
	auto old_symbols = active_symbols;
	active_symbols = &module->entity->symbols();
	for (const auto& decl : module->declarations) {
		decl->accept(*this);
	}
	active_symbols = old_symbols;
}
void type_checker::visit(std::shared_ptr<binary_expression> expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);

	type_ptr lty = nullptr;
	type_ptr rty = nullptr;

	auto left_entity = expr->left->entity(*active_symbols);
	auto right_entity = expr->right->entity(*active_symbols);
	if (left_entity == entity::UNRESOLVED || right_entity == entity::UNRESOLVED) {
		// cannot resolve one of the entities, skip further checks
		return;
	}

	if (left_entity == nullptr) {
		lty = expr->left->type();
		if (!lty) {
			ERROR(ERR_INTERNAL_ERROR, expr->left->span, "Type Checker", "LHS of binary expression has no type or entity");
			return;
		}
	}
	else {
		if (left_entity->kind() != ENTITY_VARIABLE) {
			throw;
		}
		lty = left_entity->as_variable()->var_type();
	}

	if (right_entity == nullptr) {
		rty = expr->left->type();
		if (!rty) {
			ERROR(ERR_INTERNAL_ERROR, expr->left->span, "Type Checker", "RHS of binary expression has no type or entity");
			return;
		}
	}
	else {
		if (right_entity->kind() != ENTITY_VARIABLE) {
			throw;
		}
		rty = right_entity->as_variable()->var_type();
	}

	// if either type is UNKNOWN we can just ignore it, error flagged elsewhere
	if (lty == data_type::UNKNOWN || rty == data_type::UNKNOWN) {
		return;
	}

	const auto& builtin_operators = get_core_operators();
	// if both sides are primitive types, check if a built in operator exists
	if (lty->is_primitive() && rty->is_primitive()) {
		for (const auto& op : builtin_operators) {
			if (op.matches(lty, expr->oparator, rty)) {
				expr->result_type = op.result_type;
				return;
			}
		}
		ERROR(ERR_NO_MATCHING_OPERATOR, expr->span, lty->name().c_str(), rty->name().c_str());
		return;
	}
	else if (lty->is_enum() && rty->is_enum()) {
		// we can compare enums if they are the same type
		if (*lty == rty && expr->oparator == TT_EQUAL || expr->oparator == TT_NOT_EQUAL) {
			expr->result_type = to_data_type(DT_BOOL);
			return;
		}
	}
	else {
		// if at least one side is custom, we can check user-defined operators
		if (auto left_custom = lty->as_custom()) {
			if (!left_custom->declaration) {
				ERROR(ERR_TYPE_NOT_DEFINED, expr->span, left_custom->name().c_str());
				return;
			}
			for (const auto& op : left_custom->declaration->operators) {
				if (op->matches(lty, expr->oparator, rty)) {
					expr->result_type = op->result_type;
					return;
				}
			}
		}
		if (auto right_custom = rty->as_custom()) {
			if (!right_custom->declaration) {
				ERROR(ERR_TYPE_NOT_DEFINED, expr->span, right_custom->name().c_str());
				return;
			}
			for (const auto& op : right_custom->declaration->operators) {
				if (op->matches(lty, expr->oparator, rty)) {
					expr->result_type = op->result_type;
					return;
				}
			}
		}
	}
	// no built-in or user-defined operators available
	ERROR(ERR_NO_MATCHING_OPERATOR_BUILTIN_USER, expr->span, lty->name().c_str(), rty->name().c_str());
}
void type_checker::visit(std::shared_ptr<assignment_expression> expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
	auto left_type = expr->left->type();
	auto right_type = expr->right->type();

	// cannot assign to a const variable
	auto entity = expr->left->entity(*active_symbols);
	if (entity->kind() == ENTITY_VARIABLE) {
		if (entity->as_variable()->is_const()) {
			ERROR(ERR_CONST_ASSIGNMENT, expr->span, entity->name().c_str());
			return;
		}
	}

	// cannot assign to an rvalue
	if (expr->left->is_rvalue()) {
		ERROR(ERR_ASSIGNMENT_TO_RVALUE, expr->span);
		return;
	}

	// if either type is UNKNOWN we can just ignore it for now, as it will
	// make some kind of error elsewhere
	if (left_type == data_type::UNKNOWN || right_type == data_type::UNKNOWN) {
		return;
	}

	// mismatch assignment
	if (*left_type != right_type && !is_valid_conversion(right_type, left_type, true, expr->right->span)) {
		ERROR(ERR_TYPE_ASSIGNMENT_MISMATCH, expr->span, left_type->name().c_str(), right_type->name().c_str());
		return;
	}
}
void type_checker::visit(std::shared_ptr<address_of_expression> expr) {
	expr->value->accept(*this);
	if (expr->is_rvalue()) {
		ERROR(ERR_ADDRESS_OF_RVALUE, expr->span);
		return;
	}
}
void type_checker::visit(std::shared_ptr<deref_expression> expr) {
	expr->value->accept(*this);
	if (!expr->value->type()->is_pointer()) {
		ERROR(ERR_DEREFERENCE_OF_NON_POINTER, expr->span);
		return;
	}
}
void type_checker::visit(std::shared_ptr<unary_expression> expr) {
	expr->operand->accept(*this);
	auto operand_type = expr->operand->type();
	switch (expr->oparator) {
	case TT_NOT:
		if (!operand_type->is_primitive() || operand_type->primitive != DT_BOOL) {
			ERROR(ERR_LOGICAL_NOT_BOOLEAN_ONLY, expr->span);
			return;
		}
		break;
	case TT_SUBTRACT:
		if (!operand_type->is_primitive() || !operand_type->is_numeric()) {
			ERROR(ERR_NEGATE_NUMERIC_ONLY, expr->span);
			return;
		}
		break;
	case TT_INCREMENT:
		if (!operand_type->is_primitive() || !operand_type->is_integral()) {
			ERROR(ERR_INCREMENT_INTEGER_ONLY, expr->span);
			return;
		}
		break;
	case TT_DECREMENT:
		if (!operand_type->is_primitive() || !operand_type->is_integral()) {
			ERROR(ERR_DECREMENT_INTEGER_ONLY, expr->span);
			return;
		}
		break;
	}
}
void type_checker::visit(std::shared_ptr<identifier_expression> expr) {
	expr->entity_ref.resolve(*active_symbols);
}
void type_checker::visit(std::shared_ptr<index_expression> expr) {
	expr->base->accept(*this);
	if (!expr->base->type()->is_indexable()) {
		ERROR(ERR_BASE_NOT_INDEXABLE, expr->span);
		return;
	}
	expr->indexer->accept(*this);
	auto indexer_type = expr->indexer->type();
	if (!indexer_type->is_primitive() || !indexer_type->is_integral()) {
		ERROR(ERR_INDEXER_NOT_INTEGER, expr->indexer->span);
		return;
	}
}
void type_checker::visit(std::shared_ptr<cast_expression> expr) {
	expr->expr->accept(*this);

	auto from = expr->expr->type();
	auto& to = expr->cast_type;

	if (!is_valid_conversion(from, to, false, expr->span)) {
		ERROR(ERR_NO_CONVERSION_EXISTS, expr->span, from->name().c_str(), to->name().c_str());
		return;
	}
}
void type_checker::visit(std::shared_ptr<member_expression> expr) {
	expr->object->accept(*this);

	if (expr->is_static_access()) {
		// all static member accesses are resolved in the name resolver
		// we ONLY need to resolve here if it involves types
		return;
	}

	// instance access only allowed on variables
	auto entity = expr->object->entity(*active_symbols);
	if (entity->kind() != ENTITY_VARIABLE) {
		ERROR(ERR_MEMBER_ACCESS_NOT_ON_VARIABLE, expr->span);
		return;
	}

	auto type = entity->as_variable()->var_type();
	if (type == data_type::UNKNOWN) {
		// some other error occured
		return;
	}

	// single auto-dereference
	type = auto_deref(type);

	if (!member_access_allowed(type)) {
		if (auto enm = type->as_enum()) {
			ERROR(ERR_ENUM_OPTION_MEMBER_ACCESS, expr->span, type->name().c_str());
			return;
		}
		ERROR(ERR_MEMBER_ACCESS_ON_NONCOMPOSITE, expr->span, type->name().c_str());
		return;
	}

	bool found = false;
	if (auto custom = type->as_custom()) {
		auto entity = type_entity::get(custom->declaration);
		do {
			auto var = entity->symbols.get_field(expr->member);
			if (var) {
				expr->entity_ref = var->ref();
				found = true;
				break;
			}
			entity = entity->base_type();
		} while (entity != nullptr);
	}
	else if (auto enm = type->as_enum()) {
		for (const auto& option : enm->declaration->options) {
			if (option->identifier == expr->member) {
				auto option_type = option->type()->clone()->as_enum();
				option_type->is_enum_option = true;
				option_type->option_identifier = option->identifier;
				expr->resolved_type = option_type;
				found = true;
				break;
			}
		}
	}
	else {
		ERROR(ERR_INTERNAL_ERROR, expr->span, "Type Checker", "Member access on unsupported composite type");
		return;
	}

	if (!found) {
		if (type->is_enum()) {
			ERROR(ERR_NO_ENUM_MEMBER_WITH_NAME, expr->span, type->name().c_str(), expr->member.c_str());
			return;
		}
		ERROR(ERR_NO_MEMBER_WITH_NAME, expr->span, type->name().c_str(), expr->member.c_str());
		return;
	}
}
void type_checker::visit(std::shared_ptr<initializer_list> init) {
	if (init->is_array_initializer) {
		type_ptr type = data_type::UNKNOWN;
		for (const auto& value : init->values) {
			value->accept(*this);
			if (type == data_type::UNKNOWN) {
				type = value->type();
			}
			else if (*type != value->type()) {
				ERROR(ERR_ARRAY_INITIALIZER_TYPE_MISMATCH, init->span, type->name().c_str(), value->type()->name().c_str());
				return;
			}
		}
		// we know the result type for array initializers
		// however, we need to remember its an array and convert accordingly
		init->result_type = make_array(type);
		return;
	}
	for (const auto& value : init->values) {
		value->accept(*this);
	}
}
void type_checker::visit(std::shared_ptr<function_call> func_call) {
	std::vector<type_ptr> arg_types;
	for (const auto& arg : func_call->args) {
		arg->accept(*this);
		arg_types.push_back(arg->type());
	}
	std::vector<type_ptr> generic_types;
	for (const auto& gen : func_call->generic_args) {
		// add to generics list
		generic_types.push_back(gen);
	}

	// we need to resolve constructor candidates here in case its a generic type
	if (func_call->is_constructor) {
		auto& ctor_type = func_call->ctor_type;
		if (!ctor_type) {
			ERROR(ERR_INTERNAL_ERROR, func_call->span, "Type Checker", "Constructor type not set");
			return;
		}

		if (ctor_type->is_generic && !ctor_type->is_generic_instance) {
			// instantiate generic type
			ctor_type = unbox_generic_type(ctor_type, generic_types);
			if (!ctor_type) {
				ERROR(ERR_INTERNAL_ERROR, func_call->span, "Type Checker", "Failed to unbox generic type for constructor");
				return;
			}
		}

		// get constructor candidates
		auto ctor_candidates = get_ctor_candidates(ctor_type, func_call->args.size());
		if (ctor_candidates.empty()) {
			ERROR(ERR_NO_MATCHING_CONSTRUCTOR, func_call->span, func_call->identifier.c_str());
			return;
		}
		func_call->declaration_candidates = ctor_candidates;
	}

	// we need to resolve candidates here if the function call
	// is a method, as it cant be done in the name resolver pass
	if (func_call->is_method()) {
		// resolve the object
		func_call->callee->accept(*this);

		auto type = func_call->callee->type();
		if (!method_access_allowed(type)) {
			// should change this error really as it could be composite with no members like a Foo** etc.
			ERROR(ERR_METHOD_ACCESS_ON_NONCOMPOSITE, func_call->span, type->name().c_str());
			return;
		}

		if (!type->is_custom()) {
			// if this is ever thrown, it means there must be a problem with method_access_allowed
			ERROR(ERR_INTERNAL_ERROR, func_call->span, "Type Checker", "Method call on non-custom type");
		}

		// find method - dont check for return type as its unknown in a call
		auto method_candidates = get_method_candidates(type->as_custom()->declaration, func_call->identifier, func_call->args.size());
		if (method_candidates.empty()) {
			ERROR(ERR_NO_MATCHING_METHOD, func_call->span, func_call->identifier.c_str());
			return;
		}
		func_call->declaration_candidates = method_candidates;
	}

	// TEMPORARY
	if (func_call->identifier == "printf") return;

	if (!func_call->declaration_candidates.empty()) {
		// check any candidates match arguments provided
		std::vector<candidate_score> matches;
		for (const auto& candidate : func_call->declaration_candidates) {
			auto expected_types = candidate->get_param_types();
			if (expected_types.size() != arg_types.size()) {
				ERROR(ERR_INTERNAL_ERROR, func_call->span, "Type Checker", "Argument count for candidate doesnt match");
				continue; // argument count doesn't match
			}

			// set generics (for scoring)
			if (candidate->is_generic) {
				// push generic substitutions
				generic_substitution_stack.push_back({});
				for (size_t i = 0; i < func_call->generic_args.size(); i++) {
					generic_substitution_stack.back().push_back(func_call->generic_args[i]);
				}
			}

			int score = score_candidate(candidate, arg_types);
			if (score > 0) {
				matches.push_back({ candidate, score });
			}

			// reset generics
			if (candidate->is_generic) {
				generic_substitution_stack.pop_back();
			}
		}
		if (matches.empty()) {
			if (func_call->is_constructor) {
				ERROR(ERR_NO_MATCHING_CONSTRUCTOR, func_call->span, func_call->identifier.c_str());
			}
			else if (func_call->is_method()) {
				ERROR(ERR_NO_MATCHING_METHOD, func_call->span, func_call->identifier.c_str());
			}
			else {
				ERROR(ERR_NO_MATCHING_FUNCTION, func_call->span, func_call->identifier.c_str());
			}
			return;
		}
		else {
			// sort matches by score
			std::sort(matches.begin(), matches.end(), [](const candidate_score& a, const candidate_score& b) {
				return a.score > b.score;
			});
			// if top 2 scores are the same, we have an ambiguity error
			if (matches.size() > 1 && matches[0].score == matches[1].score) {
				if (func_call->is_constructor) {
					ERROR(ERR_AMBIGUOUS_CONSTRUCTOR_CALL, func_call->span, func_call->identifier.c_str());
				}
				else if (func_call->is_method()) {
					ERROR(ERR_AMBIGUOUS_METHOD_CALL, func_call->span, func_call->identifier.c_str());
				}
				else {
					ERROR(ERR_AMBIGUOUS_FUNCTION_CALL, func_call->span, func_call->identifier.c_str());
				}
				return;
			}
			std::shared_ptr<function_declaration> best_match = matches[0].candidate;
			if (best_match->is_generic) {
				best_match = unbox_generic_func(best_match, generic_types);
				if (!best_match) {
					ERROR(ERR_INTERNAL_ERROR, func_call->span, "Type Checker", "Failed to unbox generic function");
					return;
				}
			}
			func_call->declaration = best_match;
		}
	}
	// no candidates, name resolver will already emit an error
}
void type_checker::visit(std::shared_ptr<if_statement> if_stmt) {
	if_stmt->condition->accept(*this);

	if_stmt->then_block->accept(*this);
	if (if_stmt->else_node) {
		if_stmt->else_node->accept(*this);
	}

	auto cond_type = if_stmt->condition->type();
	if (cond_type == data_type::UNKNOWN) {
		// assume error has already been reported
		return;
	}

	if (!if_stmt->condition->type()->is_primitive() || if_stmt->condition->type()->primitive != DT_BOOL) {
		ERROR(ERR_IF_CONDITION_NOT_BOOLEAN, if_stmt->span);
		return;
	}
}
void type_checker::visit(std::shared_ptr<inline_if> inline_if) {
	inline_if->condition->accept(*this);
	inline_if->statement->accept(*this);

	if (!inline_if->condition->type()->is_primitive() || inline_if->condition->type()->primitive != DT_BOOL) {
		ERROR(ERR_IF_CONDITION_NOT_BOOLEAN, inline_if->span);
		return;
	}
}
void type_checker::visit(std::shared_ptr<for_loop> for_loop) {
	if (for_loop->initializer) {
		for_loop->initializer->accept(*this);
	}
	// condition should always be a boolean expression
	if (for_loop->condition) {
		for_loop->condition->accept(*this);
		if (!for_loop->condition->type()->is_primitive() || for_loop->condition->type()->primitive != DT_BOOL) {
			ERROR(ERR_FOR_CONDITION_NOT_BOOLEAN, for_loop->span);
			return;
		}
	}
	if (for_loop->increment) {
		for_loop->increment->accept(*this);
	}
	for_loop->body->accept(*this);
}
void type_checker::visit(std::shared_ptr<while_loop> while_loop) {
	while_loop->condition->accept(*this);
	if (!while_loop->condition->type()->is_primitive() || while_loop->condition->type()->primitive != DT_BOOL) {
		ERROR(ERR_WHILE_CONDITION_NOT_BOOLEAN, while_loop->span);
		return;
	}
	while_loop->body->accept(*this);
}
void type_checker::visit(std::shared_ptr<return_statement> ret) {
	if (!current_function) {
		return;
	}

	if (ret->is_conditional()) {
		ret->condition->accept(*this);
		auto cond_type = ret->condition->type();
		if (!cond_type->is_primitive() || cond_type->primitive != DT_BOOL) {
			ERROR(ERR_IF_CONDITION_NOT_BOOLEAN, ret->span);
			return;
		}
	}

	check_type(current_function->return_type);

	type_ptr func_type = current_function->return_type;
	// if its an override, we should use the base function return type as overrides
	// do not redefine it
	if (current_function->is_override && current_function->overridden_function) {
		func_type = current_function->overridden_function->return_type;
	}
	if (ret->value) {
		// check if its a constructor
		if (current_function->is_constructor) {
			ERROR(ERR_CONSTRUCTOR_RETURNS_VALUE, ret->span);
			return;
		}

		// check if function is void
		if (func_type->is_primitive() && func_type->is_void()) {
			ERROR(ERR_VOID_FUNCTION_RETURNS_VALUE, ret->span, current_function->identifier.c_str());
			return;
		}

		ret->value->accept(*this);
		// ensure types match
		auto ret_type = ret->value->type();
		if (ret_type == data_type::UNKNOWN) {
			// assume error has already been reported
			return;
		}
		if (!ret_type || !is_valid_conversion(ret_type, func_type, true, ret->span)) {
			ERROR(ERR_FUNCTION_RETURN_TYPE_MISMATCH, ret->span, current_function->identifier.c_str(), func_type->name().c_str(), ret_type->name().c_str());
			return;
		}
	}
	else if (!current_function->is_constructor) {
		// ensure function is void
		if (!func_type->is_primitive() || !func_type->is_void()) {
			ERROR(ERR_FUNCTION_MUST_RETURN_VALUE, ret->span, current_function->identifier.c_str());
			return;
		}
	}
}

bool type_checker::member_access_allowed(type_ptr type) {
	if (type->is_custom()) return true;
	if (auto enm = type->as_enum()) {
		if (enm->is_enum_option) return false;
		return true;
	}
	return false;
}
bool type_checker::method_access_allowed(type_ptr type) {
	if (type->is_custom()) return true;
	return false;
}
bool type_checker::is_valid_conversion(type_ptr from, type_ptr to, bool implicit, code_span span) {
	if (*from == to) {
		// always convertable
		return true;
	}
	if (from->is_custom()) {
		return is_valid_upcast(from, to, span);
	}
	else if (from->is_primitive()) {
		const auto& builtin_conversions = get_core_conversions(from->primitive);
		for (const auto& conv : builtin_conversions) {
			if (*conv.to == to) {
				if (implicit == conv.implicit) {
					return true;
				}
				else if (!implicit && conv.implicit) {
					WARN(WARN_CAST_UNNEEDED, span, from->name().c_str(), to->name().c_str());
					return true;
				}
				else /* implicit && !conv.implicit */ {
					ERROR(ERR_CONVERSION_EXPLICIT_CAST_REQUIRED, span, from->name().c_str(), to->name().c_str());
					return false;
				}
			}
		}
	}
	return false;
}
bool type_checker::is_valid_upcast(type_ptr from, type_ptr to, code_span span) {
	if (!from->is_pointer() || !to->is_pointer()) {
		// both must be pointers for a valid upcast
		return false;
	}
	auto from_ptr = from->as_pointer();
	auto to_ptr = to->as_pointer();
	auto from_base = from_ptr->base_type->as_custom();
	auto to_base = to_ptr->base_type->as_custom();
	if (!from_base || !to_base) {
		// both must be custom types and only single level pointers
		return false;
	}

	// check up the inheritance chain of from_base to see if we reach to_base
	std::unordered_set<std::string> inherited;
	std::shared_ptr<const type_declaration> current = from_base->declaration;
	while (current) {
		if (inherited.count(current->identifier)) {
			// circular inheritance chain, should have been caught earlier
			break;
		}
		for (const auto& base : current->base_types) {
			inherited.insert(base->name());
		}
		current = current->base_type;
	}
	return inherited.count(to_base->name()) > 0;
}
bool type_checker::is_valid_entry_point(std::shared_ptr<function_declaration> func) {
	// must be a global function
	if (func->is_method || func->is_override) {
		return false;
	}
	// must match the entry point name
	if (func->identifier != language_constants::ENTRY_POINT) {
		return false;
	}
	// must return int
	if (!func->return_type->is_primitive() || func->return_type->primitive != DT_I32) {
		return false;
	}
	// must have no parameters
	if (!func->parameters.empty()) {
		return false;
	}
	// TODO: add support for other versions like int entry(string[] args)
	return true;
}

int type_checker::score_candidate(std::shared_ptr<function_declaration> candidate, const std::vector<type_ptr>& arg_types) {
	auto expected = candidate->get_param_types();

	if (expected.empty() && arg_types.empty()) {
		return 2; // perfect match for no-arg functions
	}

	// mostly for safety, should never happen
	if (expected.size() != arg_types.size()) return 0;
	int score = 0;

	for (size_t i = 0; i < expected.size(); ++i) {
		check_type(expected[i]);

		if (*expected[i] == arg_types[i]) {
			score += 2; // exact match
		}
		else if (is_valid_conversion(arg_types[i], expected[i], true, candidate->span)) {
			score += 1; // valid implicit conversion
		}
		else {
			return 0; // no match
		}
	}

	return score;
}

void type_checker::check_type(type_ptr& type) {
	if (type->is_custom() && type->generic_args.size() > 0) {
		unbox_type(type);
		return;
	}
	else if (auto ptr = type->as_pointer()) {
		check_type(ptr->base_type);
		return;
	}
	else if (auto arr = type->as_array()) {
		check_type(arr->base_type);
		if (auto& sz = arr->size_expression) {
			sz->accept(*this);

			if (!sz->is_constant()) {
				ERROR(ERR_ARRAY_SIZE_MUST_BE_CONSTANT, arr->size_expression->span);
				return;
			}

			auto size_expr_type = arr->size_expression->type();
			if (!size_expr_type->is_integral()) {
				ERROR(ERR_ARRAY_SIZE_MUST_BE_INTEGER, arr->size_expression->span);
				return;
			}
			return;
		}
	}
	else if (auto gen = type->as_generic()) {
		const auto& cur_subs = generic_substitution_stack.back();
		if (cur_subs.size() < gen->generic_param_index) {
			ERROR(ERR_INTERNAL_ERROR, gen->span, "Type Checker", "Generic substitution index out of bounds");
			return;
		}
		type = cur_subs[gen->generic_param_index];
		return;
	}
}
void type_checker::unbox_type(type_ptr& type) {
	auto custom = type->as_custom();
	for (auto& arg : type->generic_args) {
		check_type(arg);
	}
	custom->declaration = unbox_generic_type(custom->declaration, type->generic_args);
}

type_ptr type_checker::auto_deref(type_ptr type) {
	if (auto ptr = type->as_pointer()) {
		return ptr->base_type;
	}
	return type;
}

std::shared_ptr<function_declaration> type_checker::unbox_generic_func(std::shared_ptr<function_declaration> func, const std::vector<type_ptr>& types) {
	if (types.size() != func->generics.size()) {
		ERROR(ERR_INTERNAL_ERROR, func->span, "Type Checker", "Generic function unboxing called with incorrect number of types");
		return nullptr;
	}

	// check cache
	if (generic_function_instances.find(func) != generic_function_instances.end()) {
		for (const auto& [gen_types, decl] : generic_function_instances[func]) {
			if (gen_types.size() != types.size()) continue;
			bool match = true;
			for (size_t i = 0; i < gen_types.size(); i++) {
				if (*gen_types[i] != types[i]) { match = false; break; }
			}
			if (match) return decl;
		}
	}

	// clone
	auto new_func = std::dynamic_pointer_cast<function_declaration>(func->clone());
	new_func->is_generic_instance = true;

	// cache instance
	generic_function_instances[func][types] = new_func;

	// substitute concrete types in place
	generic_substitutor substitutor(pass_unit, types);
	new_func->accept(substitutor);

	// overlay table
	const symbol_table* old_table = active_symbols;
	overlay_table overlay(*active_symbols);

	// remap func
	overlay.remap(func->func_ref.get_id(), function_entity::get(new_func));
	// remap params
	for (size_t i = 0; i < func->parameters.size(); i++) {
		overlay.remap(func->parameters[i]->var_ref.get_id(), variable_entity::get(new_func->parameters[i]));
	}

	active_symbols = &overlay;

	// type check instance
	std::shared_ptr<function_declaration> old_current = current_function;
	new_func->accept(*this);
	current_function = old_current;

	active_symbols = old_table;

	return new_func;
}
std::shared_ptr<type_declaration> type_checker::unbox_generic_type(std::shared_ptr<type_declaration> type, const std::vector<type_ptr>& types) {
	if (types.size() != type->generics.size()) {
		ERROR(ERR_INTERNAL_ERROR, type->span, "Type Checker", "Generic type unboxing called with incorrect number of types");
		return nullptr;
	}

	// check if already unboxed
	if (generic_type_instances.find(type) != generic_type_instances.end()) {
		for (const auto& [gen_types, decl] : generic_type_instances[type]) {
			if (gen_types.size() != types.size()) continue;
			bool match = true;
			for (size_t i = 0; i < gen_types.size(); i++) {
				if (*gen_types[i] != types[i]) { match = false; break; }
			}
			if (match) return decl;
		}
	}

	// clone the type
	auto new_type = std::dynamic_pointer_cast<type_declaration>(type->clone());
	new_type->is_generic_instance = true;

	// store instance
	generic_type_instances[type][types] = new_type;

	// ensure constructors point to the new type
	for (const auto& ctor : new_type->constructors) {
		ctor->return_type = new_type->type();
	}

	// type check instance
	new_type->accept(*this);

	return new_type;
}
