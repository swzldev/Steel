#pragma once

#include <string>
#include <vector>

#include "token.h"
#include "../parser/compilation_pass.h"

enum _lexer_flags {
	LF_NONE = 0,
	LF_TEXT = 1 << 0,
	LF_MULTILINE = 1 << 1,
};
typedef int lexer_flags;

class lexer : public compilation_pass {
public:
	lexer(const std::string& source, std::shared_ptr<class compilation_unit > unit)
		: source(source), compilation_pass(unit) {
	}

	std::vector<token> tokenize();

private:
	std::string source;
	size_t current = 0;
	size_t line = 1;
	size_t column = 1;
	lexer_flags flags = LF_NONE;
	std::vector<token> tokens;

	bool check_for_number(std::string& src, size_t& index);
	bool check_for_char(std::string& src, size_t& index);
	bool check_for_string(std::string& src, size_t& index);

	void add_token(const std::string& value);
	void add_token(const std::string& value, token_type type);
};