#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

#include "compiler.h"
#include "utils/console_colors.h"
#include "stproj/stproj_file.h"
#include "config/console_args.h"
#include "config/compile_config.h"
#include "error/error.h"
#include "error/error_printer.h"
//#include "interpreter/interpreter.h"
#include "codegen/codegen.h"
#include "codegen/ir/llvm_ir_holder.h"
#include "output/output_config.h"
#include "output/code_outputter.h"

int main(int argc, char** argv) {
	console_colors::enable();

	console_args args(argc, argv);

	if (args.count() < 2) {
		std::cerr << "Usage: SteelCompiler <project_file>" << std::endl;
		std::cin.get();
		return 1;
	}

	std::string project_path = args.get_arg(1);
	if (!std::filesystem::exists(project_path)) {
		std::cerr << "Project file: " << project_path << " does not exist." << std::endl;
		std::cin.get();
		return 1;
	}
	std::string project_dir = std::filesystem::path(project_path).parent_path().string();

	stproj_file proj = stproj_file::load(project_path);
	compiler compiler(proj.sources);

	compile_config cmp_cfg{};

	// -- CONFIG --
	cmp_cfg.print_src = false;
	cmp_cfg.print_tokens = false;
	cmp_cfg.print_tokenized_input = false;
	cmp_cfg.print_ast = false;

	compiler.compile(cmp_cfg);

	if (compiler.has_errors()) {
		std::cerr << "\033[1;31mCompilation failed:\033[0m\n";
		error_printer::print_errors(compiler.get_errors());
		std::cin.get();
		return 1;
	}
	if (compiler.has_warnings()) {
		// print warnings
	}

	codegen codegen(compiler.module_manager, compiler.compilation_units);
	std::vector<llvm_ir_holder> ir_holders = codegen.generate_ir();

	output_config op_cfg{};
	code_outputter outputter(project_dir, op_cfg);

	for (const auto& ir_module : ir_holders) {
		// CAN cause duplicates - will deal with later
		std::shared_ptr<source_file> src = ir_module.owning_unit->source_file;
		std::string filename = src->name() + ".ll";
		outputter.output_il(ir_module.ir, filename);

		std::cout << "Output IR: " << src->name() << " -> " << filename << std::endl;
	}

	std::cout << console_colors::GREEN << "Compilation succeeded." << console_colors::RESET << std::endl;
	
	//interpreter interpreter(compiler.module_manager);
	//interpreter.begin_execution();

	std::cin.get();
}