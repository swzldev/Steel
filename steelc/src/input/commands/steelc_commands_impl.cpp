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
	output::err("Unexpected argument: {}", console_colors::RED, arg);
}

bool steelc_commands_impl::help_command_handler(const console_args& args) {
	output::print("Commands:\n", console_colors::BOLD);
	output::print("steelc --help - Shows this help message.\n");
	output::print("steelc --version - Displays the current version of steelc that is installed.\n");
	output::print("steelc build <stproj file> - Builds the project at path <stproj_file>.\n");
	output::print("steelc project <option> - Project manipulation tools. Differs by <option>.\n");
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
		bad_usage("steelc build <stproj file> [args]\n");
		return false;
	}

	// TODO: check additional arguments and report any errors

	project_builder builder;
	if (!builder.load_project(*args.get_arg(2) /* safe deref */)) {
		// dont print a message here (the builder does it)
		return false;
	}

	std::string build_command = "clang ${IRS} -o ${OUTPUT_DIR}" + builder.project_filename() + ".exe";
	builder.post_build_commands.push_back(build_command);

	return builder.build_project();
}
bool steelc_commands_impl::project_command_handler(const console_args& args) {
	if (args.count() < 3) {
		bad_usage("steelc project <option> [args]\n");
		return false;
	}

	std::string command = *args.get_arg(2);
	if (command == "new") {
		if (args.count() < 4) {
			bad_usage("steelc project new <name> [args]\n");
			return false;
		}

		std::string name = *args.get_arg(3);
		stproj_generator generator;
		return generator.generate_new_project(name, std::filesystem::current_path());
	}
	else {
		bad_usage("steelc project <option> [args]\n");
		return false;
	}
}
