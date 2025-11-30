#include <iostream>
#include <filesystem>
#include <string>

#include "building/project_builder.h"
#include "utils/console_colors.h"
#include "config/console_args.h"
#include "output/output.h"

int main(int argc, char** argv) {
	console_colors::enable();
	console_args args(argc, argv);

	if (args.count() < 2) {
		output::err("Usage: SteelCompiler <project_file> <args>\n", console_colors::RED);
		return 1;
	}

	std::string project_path = *args.get_arg(1); // safe dereference, checked count above
	if (!std::filesystem::exists(project_path)) {
		output::err("Project file: \"{}\" does not exist.\n", console_colors::RED, project_path);
		return 1;
	}

	project_builder builder;
	
	std::string build_command = "clang ${IRS} -o " + builder.project_filename() + ".exe";
	builder.post_build_commands.push_back(build_command);

	bool build_result = builder.build_project(project_path);

	output::shutdown();

	return build_result ? 0 : 1;
}