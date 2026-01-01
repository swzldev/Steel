#include <string>

#include <harness/test_defs.h>

#include <lexer/token.h>
#include <lexer/lexer.h>

TEST(lexer_basic_tokens) {
	std::string src1 = "let const int = 3";
	lexer lx1(src1, nullptr);
	auto tokens = lx1.tokenize();
	if (tokens.size() != 6 /* account for EOF */) {
		return 1;
	}
	if (tokens[0].type != TT_LET || tokens[0].value != "let") {
		return 2;
	}
	if (tokens[1].type != TT_CONST || tokens[1].value != "const") {
		return 3;
	}
	if (tokens[2].type != TT_IDENTIFIER || tokens[2].value != "int") {
		return 4;
	}
	if (tokens[3].type != TT_ASSIGN || tokens[3].value != "=") {
		return 5;
	}
	if (tokens[4].type != TT_INTEGER_LITERAL || tokens[4].value != "3") {
		return 6;
	}
	if (tokens[5].type != TT_EOF) {
		return 7;
	}
	return 0;
}