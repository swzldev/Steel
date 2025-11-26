#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib>
#include <memory>
#include <chrono>

#include "compiler.h"
#include "utils/console_colors.h"
#include "stproj/stproj_file.h"
#include "config/console_args.h"
#include "config/compile_config.h"
#include "config/bad_arg_exception.h"
#include "error/error_printer.h"
#include "codegen/codegen.h"
#include "codegen/ir/llvm_ir_holder.h"
#include "output/output_config.h"
#include "output/code_outputter.h"

int main(int argc, char** argv) {
	console_colors::enable();

	console_args args(argc, argv);

	if (args.count() < 2) {
		std::cerr << "Usage: SteelCompiler <project_file> <args>" << std::endl;
		return 1;
	}

	std::string project_path = *args.get_arg(1); // safe dereference, checked count above
	if (!std::filesystem::exists(project_path)) {
		std::cerr << "Project file: " << project_path << " does not exist." << std::endl;
		return 1;
	}
	std::string project_dir = std::filesystem::path(project_path).parent_path().string();

	stproj_file proj = stproj_file::load(project_path);
	compiler compiler(proj.sources);

	compile_config cmp_cfg{};
	try {
		cmp_cfg = compile_config::from_console_args(args);
	}
	catch (const bad_arg_exception& e) {
		std::cerr << console_colors::RED << "Error: " << console_colors::RESET << e.what() << "." << std::endl;
		return 1;
	}

	auto start = std::chrono::high_resolution_clock::now();
	if (compiler.compile(cmp_cfg)) {
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = now - start;
		std::cout << console_colors::BOLD << console_colors::GREEN << "Compilation succeeded. " << console_colors::RESET
			<< console_colors::DIM << "(Took " << elapsed.count() << " seconds)"
			<< console_colors::RESET << std::endl;
	}


	if (compiler.has_errors()) {
		std::cerr << console_colors::BOLD << console_colors::RED << "Compilation failed:" << console_colors::RESET << std::endl;
		error_printer::print_errors(compiler.get_errors());
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
	}

	std::cout << "\nStarting build... (clang)" << std::endl;

	std::string build_command = "clang ";
	for (const auto& ir_module : ir_holders) {
		std::shared_ptr<source_file> src = ir_module.owning_unit->source_file;
		std::string filename = src->name() + ".ll";
		build_command += "\"" + (outputter.get_intermediate_dir() / filename).string() + "\" ";
	}
	// will change this in the future so it doesnt default to .exe
	build_command += "-o \"" + (outputter.get_output_dir() / (proj.filename_no_extension().string() + ".exe")).string() + "\"";

	std::filesystem::current_path(project_dir);
	if (std::system(build_command.c_str()) == 0) {
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = now - start;
		std::cout << console_colors::BOLD << console_colors::GREEN << "Build succeeded. " << console_colors::RESET
			<< console_colors::DIM << "(Took " << elapsed.count() << " seconds)"
			<< console_colors::RESET << std::endl;
	}
	else {
		std::cerr << console_colors::RED << "Build failed." << console_colors::RESET << std::endl;
	}
}