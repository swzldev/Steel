#include <Windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <unordered_map>

#include "compiler.h"
#include "stproj/stproj_file.h"
#include "lexer/token_utils.h"
#include "error/error.h"
#include "interpreter/interpreter.h"

#undef min
#undef max

static void enable_ansi_escape_codes() {
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole != INVALID_HANDLE_VALUE) {
		DWORD mode;
		GetConsoleMode(hConsole, &mode);
		mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hConsole, mode);
	}
#endif
}

static void print_errors(const std::vector<error>& errors) {
	for (const auto& error : errors) {
		std::string file_path = "<unknown file>";
		const std::vector<std::string>* src = nullptr;

		if (error.unit && error.unit->source_file) {
			file_path = error.unit->source_file->path;
			src = &error.unit->source_file->lines;
		}

		std::cerr << "\033[1;31mError: \033[0m"
			<< "\033[1m" << error.info.message << "\033[0m"
			<< " in \033[35m" << file_path << "\033[0m"
			<< " at \033[36mline " << error.pos.line << ", column " << error.pos.column << "\033[0m\n";

		if (src && error.pos.line > 0 && error.pos.line <= src->size()) {
			int start_line = std::max(1, (int)error.pos.line - 2);
			int end_line = error.pos.line;
			int line_num_width = std::to_string(src->size()).size();

			for (int i = start_line; i <= end_line; ++i) {
				const auto& code_line = (*src)[(size_t)i - 1];
				std::cerr << "\033[2m" << std::setw(line_num_width) << i << " | " << "\033[0m" << code_line << "\n";
				if (i == error.pos.line) {
					std::cerr << std::setw(line_num_width) << " " << " | "
						<< std::string(error.pos.column - 1, ' ')
						<< "\033[1;31m^\033[0m HERE\n";
				}
			}
			std::cerr << std::endl;
		}
		else if (src) {
			std::cerr << "\033[1;31mLine number out of range.\033[0m\n";
		}
		else {
			std::cerr << "\033[1;31mSource file not found.\033[0m\n";
		}
	}
}

int main(int argc, char** argv) {
	enable_ansi_escape_codes();

	if (argc < 2 && false) {
		std::cerr << "Usage: SteelCompiler <project_file>" << std::endl;
		std::cin.get();
		return 1;
	}

	//stproj_file proj = stproj_file::load(argv[1]);
	stproj_file proj = stproj_file::load(R"(C:\Users\maxmt\source\repos\Steel\steelc\demo\bounce\BounceDemo.stproj)");
	compiler compiler;
	compiler.compile(proj.sources);

	if (compiler.has_errors()) {
		std::cerr << "\033[1;31mCompilation failed:\033[0m\n";
		print_errors(compiler.lexer_errors);
		print_errors(compiler.parsing_errors);
		print_errors(compiler.semantic_errors);
		std::cin.get();
		return 1;
	}

	interpreter interpreter(compiler.module_manager);
	interpreter.begin_execution();

	std::cin.get();
}