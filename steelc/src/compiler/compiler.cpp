#include "compiler.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include <utils/console_colors.h>
#include <config/compile_config.h>
#include <stproj/source_file.h>
#include <lexer/lexer.h>
#include <lexer/token.h>
#include <lexer/token_utils.h>
#include <parser/parser.h>
#include <ast/compilation_unit.h>
#include <ast_passes/declaration_collector.h>
#include <ast_passes/import_resolver.h>
#include <ast_passes/type_resolver.h>
#include <ast_passes/name_resolver.h>
#include <ast_passes/type_checker.h>
#include <ast_passes/init_checker.h>
#include <ast_passes/flow_analyzer.h>
#include <ast_lowering/mir_lowerer.h>
#include <mir/mir_fwd.h>
#include <mir/mir_module.h>
#include <mir/pretty/mir_printer.h>
#include <codegen/codegen.h>
#include <codegen/codegen_result.h>
#include <codegen/codegen_config.h>
#include <codegen_llvm/llvm_code_generator.h>
#include <output/output.h>

bool compiler::compile(const compile_config& cl_cfg, const codegen_config& cg_cfg) {
	for (auto& file : sources) {
		auto unit = std::make_shared<compilation_unit>();
		unit->source_file = std::make_shared<source_file>(file);

		output::print("Compiling: ", console_colors::BLUE);
		output::print("\'{}\'\n", "", file.relative_path);

		if (PRINT_SRC) {
			output::print("Source:\n");
			output::print("{}\n\n", "", file.content);
		}

		lexer lexer(file.content, unit);
		std::vector<token> tokens = lexer.tokenize();

		if (PRINT_TOKENS) {
			output::print("Generated tokens:\n");
			for (const auto& token : tokens) {
				std::string type = to_string(token.type);
				std::string value = token.value;
				std::string line = std::to_string(token.pos.line);
				std::string col = std::to_string(token.pos.column);

				output::print("[ {}, \"{}\", ln: {}, col: {}]\n", "", type, value, line, col);
			}
			output::print("\n");
		}

		if (lexer.has_errors()) {
			auto lexer_errors = lexer.get_errors();
			errors.insert(errors.end(), lexer_errors.begin(), lexer_errors.end());
			return false;
		}

		parser parser(unit, tokens);
		parser.parse();

		if (parser.has_errors()) {
			auto parser_errors = parser.get_errors();
			errors.insert(errors.end(), parser_errors.begin(), parser_errors.end());
			return false;
		}

		// AST creation complete, collect symbols for each file
		// and map to the modules for later symbol resolution
		declaration_collector collector(unit, module_manager);
		unit->accept(collector);
		if (collector.has_errors()) {
			const auto& errs = collector.get_errors();
			errors.insert(errors.end(), errs.begin(), errs.end());
		}

		compilation_units.push_back(unit);
	}

	// after collecting symbols for each file, we can
	// proceed with all other semantic analysis passes
	for (auto& unit : compilation_units) {
		// the import resolver identifies import statements and
		// adds them to the units import table (imports are file wide)
		import_resolver import_resolver(unit, module_manager);
		unit->accept(import_resolver);
		if (import_resolver.has_errors()) {
			const auto& errs = import_resolver.get_errors();
			errors.insert(errors.end(), errs.begin(), errs.end());
		}

		// the type resolver resolves explicit known typenames e.g. 'int'
		type_resolver type_resolver(unit, module_manager);
		unit->accept(type_resolver);
		if (type_resolver.has_errors()) {
			const auto& errs = type_resolver.get_errors();
			errors.insert(errors.end(), errs.begin(), errs.end());
		}

		// the name resolver resolves all identifiers to their declarations
		name_resolver name_resolver(unit, module_manager);
		unit->accept(name_resolver);
		if (name_resolver.has_errors()) {
			const auto& errs = name_resolver.get_errors();
			errors.insert(errors.end(), errs.begin(), errs.end());
		}

		type_checker type_checker(unit, module_manager);
		unit->accept(type_checker);
		if (type_checker.has_errors()) {
			const auto& errs = type_checker.get_errors();
			errors.insert(errors.end(), errs.begin(), errs.end());
		}

		init_checker init_checker(unit);
		unit->accept(init_checker);
		if (init_checker.has_errors()) {
			const auto& errs = init_checker.get_errors();
			errors.insert(errors.end(), errs.begin(), errs.end());
		}

		flow_analyzer flow_analyzer(unit);
		unit->accept(flow_analyzer);
		if (flow_analyzer.has_errors()) {
			const auto& errs = flow_analyzer.get_errors();
			errors.insert(errors.end(), errs.begin(), errs.end());
		}

		if (errors.size() > 0) {
			return false;
		}
	}

	// lower ast to mir
	std::vector<mir_module> mir_modules;
	mir_modules.reserve(compilation_units.size());
	for (auto& unit : compilation_units) {
		mir_lowerer mir_lowerer;
		mir_modules.push_back(mir_lowerer.lower_unit(unit));
	}

	if (PRINT_MIR) {
		mir_printer printer;
		for (const auto& mod : mir_modules) {
			std::string mir_text = printer.print_module(mod);
			output::print("Generated MIR:\n");
			output::print("{}\n", "", mir_text);
		}
	}

	// generate modules for all units
	codegen codegen(mir_modules, cg_cfg);
	codegen_result = codegen.generate_all();

	return codegen_result.success;
}

std::vector<std::string> compiler::read_source(std::string& path) {
	std::vector<std::string> lines;
	std::ifstream file(path);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			std::string nline = "";
			for (char c : line) {
				if (c == '\t') {
					nline += "    ";
				}
				else {
					nline += c;
				}
			}
			lines.push_back(nline);
		}
		file.close();
	}
	else {
		std::cerr << "Error: Could not open file " << path << std::endl;
	}
	return lines;
}
