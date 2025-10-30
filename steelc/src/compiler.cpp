#include "compiler.h"

#include <fstream>
#include <iostream>

#include "stproj/source_file.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "parser/semantics/declaration_collector.h"
#include "parser/semantics/import_resolver.h"
#include "parser/semantics/type_resolver.h"
#include "parser/semantics/name_resolver.h"
#include "parser/semantics/type_checker.h"
#include "parser/semantics/init_checker.h"
#include "parser/semantics/flow_analyzer.h"
#include "codegen/codegen.h"
#include "interpreter/interpreter.h"

bool compiler::compile(compile_config cfg) {
	for (auto& file : sources) {
		auto unit = std::make_shared<compilation_unit>();
		unit->source_file = std::make_shared<source_file>(file);

		lexer lexer(file.content, unit);
		std::vector<token> tokens = lexer.tokenize();

		if (lexer.has_errors()) {
			lexer_errors = lexer.get_errors();
			return false;
		}

		parser parser(unit, tokens);
		parser.parse();

		if (parser.has_errors()) {
			parsing_errors = parser.get_errors();
			return false;
		}

		// AST creation complete, collect symbols for each file
		// and map to the modules for later symbol resolution
		declaration_collector collector(unit, module_manager);
		unit->accept(collector);
		if (collector.has_errors()) {
			const auto& errors = collector.get_errors();
			semantic_errors.insert(semantic_errors.end(), errors.begin(), errors.end());
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
			const auto& errors = import_resolver.get_errors();
			semantic_errors.insert(semantic_errors.end(), errors.begin(), errors.end());
		}

		// the type resolver resolves explicit known typenames e.g. 'int'
		type_resolver type_resolver(unit, module_manager);
		unit->accept(type_resolver);
		if (type_resolver.has_errors()) {
			const auto& errors = type_resolver.get_errors();
			semantic_errors.insert(semantic_errors.end(), errors.begin(), errors.end());
		}

		// the name resolver resolves all identifiers to their declarations
		name_resolver name_resolver(unit, module_manager);
		unit->accept(name_resolver);
		if (name_resolver.has_errors()) {
			const auto& errors = name_resolver.get_errors();
			semantic_errors.insert(semantic_errors.end(), errors.begin(), errors.end());
		}

		type_checker type_checker(unit, module_manager);
		unit->accept(type_checker);
		if (type_checker.has_errors()) {
			const auto& errors = type_checker.get_errors();
			semantic_errors.insert(semantic_errors.end(), errors.begin(), errors.end());
		}

		init_checker init_checker(unit);
		unit->accept(init_checker);
		if (init_checker.has_errors()) {
			const auto& errors = init_checker.get_errors();
			semantic_errors.insert(semantic_errors.end(), errors.begin(), errors.end());
		}

		flow_analyzer flow_analyzer(unit);
		unit->accept(flow_analyzer);
		if (flow_analyzer.has_errors()) {
			const auto& errors = flow_analyzer.get_errors();
			semantic_errors.insert(semantic_errors.end(), errors.begin(), errors.end());
		}

		if (semantic_errors.size() > 0) {
			return false;
		}
	}

	return true;
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
