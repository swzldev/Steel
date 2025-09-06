#pragma once

#include <cstdarg>
#include <memory>
#include <vector>
#include <unordered_set>

#include "compilation_pass.h"
#include "ast/ast.h"
#include "symbolics/symbol_table.h"
#include "../lexer/token.h"

class parser : public compilation_pass {
public:
	parser(std::shared_ptr<compilation_unit> unit, std::vector<token>& tokens)
		: unit(unit), tokens(tokens), compilation_pass(unit) {}

	void parse();
	ast_ptr parse_declaration();
	ast_ptr parse_constructor_declaration(token& identifier_token);
	ast_ptr parse_function_declaration(type_ptr type, token& identifier_token);
	ast_ptr parse_variable_declaration(type_ptr type, token& identifier_token);
	ast_ptr parse_type_declaration(token& kind_token);
	ast_ptr parse_constructor_call(token& type_token);
	type_ptr parse_type();
	ast_ptr parse_parameter();
	ast_ptr parse_statement();
	ast_ptr parse_block();
	ast_ptr parse_if_statement();
	ast_ptr parse_for_loop();
	ast_ptr parse_while_loop();
	ast_ptr parse_return_statement();
	ast_ptr parse_expression_statement();
	ast_ptr parse_expression();
	ast_ptr parse_binary_expression(int precedence = 0);
	ast_ptr parse_unary_expression();
	ast_ptr parse_primary_expression();
	ast_ptr parse_initializer_list();

	// helper functions
	std::vector<std::shared_ptr<expression>> parse_expression_list(token_type end = TT_RPAREN);
	std::vector<std::shared_ptr<variable_declaration>> parse_parameter_list(token_type end = TT_RPAREN);

private:
	std::shared_ptr<compilation_unit> unit;
	std::vector<token>& tokens;
	size_t current = 0;

	std::unordered_set<std::string> declared_typenames;

	std::string text_literal_to_string(token& literal_token);

	token& peek();
	token& previous();
	token& consume();
	bool match_typename();
	bool match_modifier();
	bool match(int count, ...);
	bool match(token_type type);
	bool check(token_type type);
	void advance();
	bool is_at_end();
};