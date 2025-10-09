#pragma once

#include <cstdarg>
#include <memory>
#include <vector>
#include <unordered_set>

#include "compilation_pass.h"
#include "ast/ast.h"
#include "symbolics/symbol_table.h"
#include "../lexer/token.h"

constexpr auto ENABLE_GENERICS = true;

class parser : public compilation_pass {
public:
	parser(std::shared_ptr<compilation_unit> unit, std::vector<token>& tokens)
		: unit(unit), tokens(tokens), compilation_pass(unit) {}

	void parse();
	ast_node_ptr parse_declaration();
	ast_ptr<function_declaration> parse_constructor_declaration(token& typename_token);
	ast_ptr<function_declaration> parse_function_declaration(bool is_constructor, bool is_override);
	ast_ptr<variable_declaration> parse_variable_declaration(bool is_const);
	ast_ptr<type_declaration> parse_type_declaration(token& kind_token);
	ast_ptr<type_node> parse_type();
	ast_ptr<variable_declaration> parse_parameter();
	ast_node_ptr parse_statement();
	ast_ptr<code_block> parse_block();
	ast_ptr<if_statement> parse_if_statement();
	ast_ptr<for_loop> parse_for_loop();
	ast_ptr<while_loop> parse_while_loop();
	ast_ptr<return_statement> parse_return_statement();
	ast_node_ptr parse_expression_statement();
	ast_ptr<expression> parse_expression();
	ast_ptr<expression> parse_binary_expression(int precedence = 0);
	ast_ptr<expression> parse_unary_expression();
	ast_ptr<expression> parse_primary_expression();
	ast_ptr<initializer_list> parse_initializer_list();
	ast_ptr<initializer_list> parse_array_initializer();

	// helper functions
	std::vector<ast_ptr<expression>> parse_expression_list(token_type end = TT_RPAREN);
	std::vector<ast_ptr<variable_declaration>> parse_parameter_list(token_type end = TT_RPAREN);
	std::vector<ast_ptr<generic_parameter>> parse_generics();

private:
	std::shared_ptr<compilation_unit> unit;
	std::vector<token>& tokens;
	size_t current = 0;

	token& peek();
	token& previous();
	token& consume();
	bool match_primitive();
	bool match_modifier();
	bool match(std::initializer_list<token_type> types);
	bool match(token_type type);
	bool check(token_type type);
	void advance();
	void synchronize();
	bool is_at_end();
};