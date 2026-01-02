#include <string>

#include <harness/test_defs.h>

#include <lexer/token.h>
#include <lexer/lexer.h>

TEST(lexer_basic_tokens) {
	std::string src = "let const int = 3";
	lexer lx(src, nullptr);
	auto tokens = lx.tokenize();

	// check types and values:
	if (tokens.size() != 6 /* account for EOF */) {
		return 1;
	}
	if (tokens[0].type != TT_LET || tokens[0].value != "let") {
		return 2;
	}
	if (tokens[1].type != TT_CONST || tokens[1].value != "const") {
		return 3;
	}
	if (tokens[2].type != TT_I32 || tokens[2].value != "int") {
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

TEST(lexer_token_positions) {
	std::string src = R"(let const int = 3;
printf("Hello, World!\n");)";
	lexer lx(src, nullptr);
	auto tokens = lx.tokenize();

	// starts
	if (tokens[0].span.start.line != 1 || tokens[0].span.start.column != 1) {
		return 1;
	}
	if (tokens[4].span.start.line != 1 || tokens[4].span.start.column != 17) {
		return 2;
	}
	if (tokens[5].span.start.line != 1 || tokens[5].span.start.column != 18) {
		return 3;
	}
	if (tokens[6].span.start.line != 2 || tokens[6].span.start.column != 1) {
		return 4;
	}
	if (tokens[8].span.start.line != 2 || tokens[8].span.start.column != 8) {
		return 5;
	}

	// ends
	if (tokens[0].span.end.line != 1 || tokens[0].span.end.column != 3) {
		return 6;
	}
	if (tokens[4].span.end.line != 1 || tokens[4].span.end.column != 17) {
		return 7;
	}
	if (tokens[5].span.end.line != 1 || tokens[5].span.end.column != 18) {
		return 8;
	}
	if (tokens[6].span.end.line != 2 || tokens[6].span.end.column != 6) {
		return 9;
	}
	if (tokens[8].span.end.line != 2 || tokens[8].span.end.column != 24) {
		return 10;
	}

	return 0;
}