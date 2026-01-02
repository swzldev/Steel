#include "steelc_commands_impl.h"

#include <string>
#include <filesystem>
#include <cstdlib>

#include <input/commands/flags/command_flags.h>
#include <steelc/steelc_definitions.h>
#include <output/output.h>
#include <utils/console_colors.h>
#include <utils/path_utils.h>
#include <building/build_config.h>
#include <building/project_builder.h>
#include <stproj/stproj_generator.h>

bool steelc_commands_impl::help_command_handler(const command_flags& flags) {
	output::print("Commands:\n", console_colors::BOLD);
	output::print("steelc --help - Shows this help message.\n");
	output::print("steelc --version - Displays the current version of steelc that is installed.\n");
	output::print("steelc build <stproj-file | project-dir> [<args>] - Builds the project at path <stproj-file>, or attempts to build the project file located in <project-dir>.\n");
	output::print("steelc project <option> [<args>] - Project manipulation tools. Differs by <option>.\n");
	return true;
}
bool steelc_commands_impl::version_command_handler(const command_flags& flags) {
	std::string sc_ver = STEELC_VERSION_STR;
	output::print("steelc version: {}\n", "", sc_ver);
	return true;
}
bool steelc_commands_impl::build_command_handler(const command_flags& flags) {
	std::string project_file_path = flags.get_first("<project-path>");
	std::filesystem::path project_file_path_n = path_utils::normalize(project_file_path);
	if (std::filesystem::is_directory(project_file_path_n)) {
		// attempt to find a .stproj file in the directory
		bool found = false;
		for (const auto& entry : std::filesystem::directory_iterator(project_file_path_n)) {
			if (entry.path().extension() == ".stproj") {
				project_file_path_n = path_utils::normalize(entry.path().string());
				found = true;
				break;
			}
		}
		if (!found) {
			output::err("No .stproj file found in directory: {}\n", console_colors::RED, project_file_path_n.string());
			return false;
		}
	}

	build_config cfg{};

	if (flags.has("--no-link")) {
		cfg.no_link = true;
	}
	if (flags.has("--all")) {
		cfg.build_all = true;
	}
	if (flags.has("--print-tokens")) {
		cfg.print_tokens = true;
	}
	if (flags.has("--print-mir")) {
		cfg.print_mir = true;
	}
	if (flags.has("--backend")) {
		cfg.backend = flags.get_first("--backend");
	}
	if (flags.has("--ir-format")) {
		cfg.ir_format = flags.get_first("--ir-format");
	}
	if (flags.has("--target")) {
		cfg.target_triple = flags.get_first("--target");
	}
	if (flags.has("--out")) {
		cfg.output_dir = flags.get_first("--out");
	}
	if (flags.has("--int")) {
		cfg.intermediate_dir = flags.get_first("--int");
	}
	
	project_builder builder(cfg);
	if (!builder.load_project(project_file_path_n.string())) {
		// dont print a message here (the builder does it)
		return false;
	}

	return builder.build_project();
}
bool steelc_commands_impl::project_command_handler(const command_flags& flags) {
	std::string command = flags.get_first("<option>");
	if (command == "new") {
		std::string name = flags.get_first("<project>");
		stproj_generator generator;
		return generator.generate_new_project(name, std::filesystem::current_path());
	}
	else {
		output::err("Unknown project option: {}\n", console_colors::RED, command);
	}

	return false;
}
bool steelc_commands_impl::test_command_handler(const command_flags& flags) {
#if defined(STEELC_ENABLE_TESTS)
	std::string command = "\"\"tests/steelc_tests\"\"";

	int ec = std::system(command.c_str());
	return ec == 0;
#else
	output::err("Tests are not enabled in this build of steelc.\n", console_colors::RED);
	return false;
#endif
}
