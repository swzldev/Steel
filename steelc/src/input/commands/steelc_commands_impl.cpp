#include "steelc_commands_impl.h"

#include <string>
#include <filesystem>

#include "../console_args.h"
#include "../../output/output.h"
#include "../../steelc_definitions.h"
#include "../../utils/console_colors.h"
#include "../../building/project_builder.h"
#include "../../stproj/stproj_generator.h"

static void bad_usage(const std::string& usage) {
	output::err("Usage: {}", console_colors::RED, usage);
}
static void bad_arg(const std::string& arg) {
	output::err("Unknown or unexpected argument: {}", console_colors::RED, arg);
}

bool steelc_commands_impl::help_command_handler(const console_args& args) {
	output::print("Commands:\n", console_colors::BOLD);
	output::print("steelc --help - Shows this help message.\n");
	output::print("steelc --version - Displays the current version of steelc that is installed.\n");
	output::print("steelc build <stproj-file | project-dir> [<args>] - Builds the project at path <stproj-file>, or attempts to build the project file located in <project-dir>.\n");
	output::print("steelc project <option> [<args>] - Project manipulation tools. Differs by <option>.\n");
	return true;
}
bool steelc_commands_impl::version_command_handler(const console_args& args) {
	if (args.count() >= 3) {
		bad_arg(*args.get_arg(2));
		return false;
	}

	std::string sc_ver = STEELC_VERSION_STR;
	output::print("steelc version: {}\n", "", sc_ver);
	return true;
}
bool steelc_commands_impl::build_command_handler(const console_args& args) {
	if (args.count() < 3) {
		bad_usage("steelc build <stproj-file | project-dir> [<args>]\n");
		return false;
	}

	bool no_link = false;
	for (size_t i = 3; i < args.count(); i++) {
		std::string arg = *args.get_arg(i);
		if (arg == "--no-link") {
			no_link = true;
		}
		else if (arg == "--verbose") {
			output::set_log_verbosity(LOG_VERBOSITY_HIGH);
		}
		else {
			bad_arg(arg);
			return false;
		}
	}

	std::string project_file_path = *args.get_arg(2); // safe deref
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

	project_builder builder;
	if (!builder.load_project(project_file_path)) {
		// dont print a message here (the builder does it)
		return false;
	}

#if defined(_WIN32) || defined(_WIN64)
	std::string app_ext = ".exe";
#elif defined(__APPLE__) || defined(__MACH__) || defined(__linux__) || defined(__unix__) || defined(__posix__)
	std::string app_ext = ".out";
#else
	std::string app_ext = "";
#endif

	if (!no_link) {
		std::string build_command = "clang ${IRS} -o \"${OUTPUT_DIR}" + builder.project_filename() + app_ext + "\"";
		builder.post_build_commands.push_back(build_command);
	}

	return builder.build_project();
}
bool steelc_commands_impl::project_command_handler(const console_args& args) {
	if (args.count() < 3) {
		bad_usage("steelc project <option> [<args>]\n");
		return false;
	}

	std::string command = *args.get_arg(2);
	if (command == "new") {
		if (args.count() < 4) {
			bad_usage("steelc project new <name> [<args>]\n");
			return false;
		}

		std::string name = *args.get_arg(3);
		stproj_generator generator;
		return generator.generate_new_project(name, std::filesystem::current_path());
	}
	else {
		bad_usage("steelc project <option> [<args>]\n");
		return false;
	}
}
