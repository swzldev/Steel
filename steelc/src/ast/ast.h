#pragma once

#include <lexer/token.h>

// single include for all nodes
// NOT forward declarations -- this provides the actual implementations
// for forward declarations use ast_fwd.h
#include <ast/ast_node.h>
#include <ast/compilation_unit.h>
#include <ast/declarations/declaration.h>
#include <ast/declarations/function_declaration.h>
#include <ast/declarations/variable_declaration.h>
#include <ast/declarations/type_declaration.h>
#include <ast/declarations/module_declaration.h>
#include <ast/declarations/conversion_declaration.h>
#include <ast/declarations/operator_declaration.h>
#include <ast/declarations/enum_declaration.h>
#include <ast/statements/code_block.h>
#include <ast/statements/import_statement.h>
#include <ast/statements/inline_if.h>
#include <ast/statements/control_flow/return_statement.h>
#include <ast/statements/control_flow/break_statement.h>
#include <ast/statements/control_flow/if_statement.h>
#include <ast/statements/control_flow/for_loop.h>
#include <ast/statements/control_flow/while_loop.h>
#include <ast/expressions/expression.h>
#include <ast/expressions/expression_statement.h>
#include <ast/expressions/binary_expression.h>
#include <ast/expressions/assignment_expression.h>
#include <ast/expressions/member_expression.h>
#include <ast/expressions/address_of_expression.h>
#include <ast/expressions/deref_expression.h>
#include <ast/expressions/unary_expression.h>
#include <ast/expressions/index_expression.h>
#include <ast/expressions/identifier_expression.h>
#include <ast/expressions/this_expression.h>
#include <ast/expressions/cast_expression.h>
#include <ast/expressions/initializer_list.h>
#include <ast/expressions/function_call.h>
#include <ast/expressions/literal.h>
#include <ast/generics/generic_parameter.h>
#include <ast/enums/enum_option.h>

// helper functions to create AST nodes
template<typename T, typename... Args>
inline std::shared_ptr<T> make_ast(token& ast_token, Args... args) {
	static_assert(std::is_base_of<ast_node, T>::value, "T must derive from ast_node");
	std::shared_ptr<T> node = std::make_shared<T>(std::forward<Args>(args)...);
	node->span = ast_token.span;
	return node;
}
template<typename T, typename... Args>
inline std::shared_ptr<T> make_ast(code_span span, Args... args) {
	static_assert(std::is_base_of<ast_node, T>::value, "T must derive from ast_node");
	std::shared_ptr<T> node = std::make_shared<T>(std::forward<Args>(args)...);
	node->span = span;
	return node;
}
template<typename T, typename... Args>
inline std::shared_ptr<T> make_ast(Args... args) {
	static_assert(std::is_base_of<ast_node, T>::value, "T must derive from ast_node");
	std::shared_ptr<T> node = std::make_shared<T>(std::forward<Args>(args)...);
	node->span = code_span();
	return node;
}