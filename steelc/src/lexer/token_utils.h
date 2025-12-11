#pragma once

#include <string>
#include <map>
#include <vector>

#include <lexer/token.h>

bool is_identifier(token& tk);
bool is_keyword(token& tk);
bool is_op(token& tk);
bool is_eof(token& tk);
bool is_primitive(token& tk);

bool is_keyword(const std::string& tk);
bool is_grammar(char tk);
bool is_op(const std::string& tk);

token_type get_keyword(const std::string& tk);
token_type get_grammar(char tk);
token_type get_op(const std::string& tk);

const std::vector<std::pair<std::string, token_type>>& get_operators();
token_type get_op(const std::string& prgm, int i);
int op_len(token_type tk);

const std::map<char, token_type>& get_grammars();
const std::map<std::string, token_type>& get_keywords();

int precedence_of(token_type tk);

bool is_integer_literal(const std::string& value);
bool is_float_literal(const std::string& value);
bool is_char_literal(const std::string& value);
bool is_string_literal(const std::string& value);

std::string to_string(token_type tk);

std::string get_colored_representation(std::vector<token> tokens);
std::string get_colored_representation(token& tk);