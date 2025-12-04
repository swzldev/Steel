#include "steelc_commands_impl.h"

#include <string>
#include <filesystem>

#include "flags/command_flags.h"
#include "../../output/output.h"
#include "../../steelc_definitions.h"
#include "../../utils/console_colors.h"
#include "../../building/build_config.h"
#include "../../building/project_builder.h"
#include "../../stproj/stproj_generator.h"

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
	if (std::filesystem::is_directory(project_file_path)) {
		// attempt to find a .stproj file in the directory
		bool found = false;
		for (const auto& entry : std::filesystem::directory_iterator(project_file_path)) {
			if (entry.path().extension() == ".stproj") {
				project_file_path = entry.path().string();
				found = true;
				break;
			}
		}
		if (!found) {
			output::err("No .stproj file found in directory: {}\n", console_colors::RED, project_file_path);
			return false;
		}
	}

	build_config cfg{}; // default build config for now
	project_builder builder(cfg);
	if (!builder.load_project(project_file_path)) {
		// dont print a message here (the builder does it)
		return false;
	}

#if defined(_WIN32) || defined(_WIN64)
	std::string app_ext = ".exe";
#elif defined(__APPLE__) || defined(__linux__)
	std::string app_ext = ".out";
#else
	std::string app_ext = "";
#endif

	if (!flags.has("--no-link")) {
		std::string build_command = "clang ${IRS} -o \"${OUTPUT_DIR}" + builder.project_filename() + app_ext + "\"";
		builder.post_build_commands.push_back(build_command);
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
}
