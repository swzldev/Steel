#include "project_builder.h"

#include <string>
#include <chrono>
#include <cstdlib>
#include <regex>
#include <vector>
#include <filesystem>

#include "../compiler.h"
#include "../config/compile_config.h"
#include "../utils/console_colors.h"
#include "../stproj/stproj_file.h"
#include "../stproj/bad_stproj_exception.h"
#include "../output/output.h"
#include "../error/error_printer.h"
#include "../output/output_config.h"
#include "../output/code_outputter.h"

bool project_builder::build_project(const std::string& project_path) {
	mark_build_start();

	// load stproj
	try {
		project_file = stproj_file::load(project_path);
	}
	catch (const bad_stproj_exception& err) {
		output::err("Error loading project file: ", console_colors::RED);
		output::err(err.message() + "\n");
		return false;
	}

	// compile sources
	compiler compiler(project_file.sources);
	compile_config compile_cfg{}; // TODO: generate config based on project settings + cl args

	mark_compile_start();
	if (compiler.compile(compile_cfg)) {
		output::print("Compilation succeeded. ", console_colors::BOLD + console_colors::GREEN);
		output::print("(Took {:.3f} seconds)\n", console_colors::DIM, get_compilation_time());
	}
	else {
		output::err("Compilation failed with {} errors.", console_colors::BOLD + console_colors::RED, compiler.get_error_count());
		error_printer::print_errors(compiler.get_errors());
		return false;
	}

	// output code
	output_config output_cfg{}; // will potentially modify the config later
	code_outputter codeout(project_file.parent_path().string(), output_cfg);

	for (const auto& il_h : compiler.get_generated_ir()) {
		std::string path = "./IL/" + il_h.owning_unit->source_file->relative_path + ".ll";
		codeout.output_il(il_h.ir, path);
		generated_ir_files.push_back((codeout.get_intermediate_dir() / path).string());
	}

	// run post-build commands
	for (const auto& cmd : post_build_commands) {
		if (int ec = run_build_command(cmd); ec != 0) {
			output::err("Post-build command \"{}\" failed with exit code: {}", console_colors::BOLD + console_colors::RED, cmd, ec);
			return false;
		}
	}

	output::print("Building succeeded. ", console_colors::BOLD + console_colors::GREEN);
	output::print("(Took {:.3f} seconds)\n", console_colors::DIM, get_build_time());
	return true;
}

int project_builder::run_build_command(const std::string& command) {
	return system(replace_vars(command).c_str());
}

double project_builder::get_build_time() const {
	return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - build_start).count();
}
double project_builder::get_compilation_time() const {
	return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - compile_start).count();
}

std::string project_builder::replace_vars(const std::string& str) {
	std::string remaining = str;
	std::string out;
	std::regex var_re(R"(\$\{([^}]+)\})");
	std::smatch m;

	while (std::regex_search(remaining, m, var_re)) {
		out += m.prefix().str();

		std::string key = m[1].str();
		if (key == "IRS") {
			for (size_t i = 0; i < generated_ir_files.size(); ++i) {
				if (i) out.push_back(' ');
				out += "\"" + generated_ir_files[i] + "\"";
			}
		}
		
		remaining = m.suffix().str();
	}

	out += remaining;
	return out;
}
