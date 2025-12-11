#include <string>

#include <utils/console_colors.h>
#include <output/output.h>
#include <input/console_args.h>
#include <input/commands/steelc_command_manager.h>
#include <input/commands/steelc_command.h>

int main(int argc, char** argv) {
	output::init();
	console_args args(argc, argv);

	if (args.count() < 2) {
		output::err("Usage: steelc <command> [args]\n", console_colors::RED);
		return 1;
	}
	std::string command = *args.get_arg(1); // safe deref

	steelc_command_manager cmd_manager;
	auto cmd = cmd_manager.get_command(command);
	if (!cmd) {
		output::err("Error: unknown command: {}\n", console_colors::RED, command);
		return 1;
	}

	bool command_result = cmd->execute(args.slice(2));

	output::shutdown();

	return command_result ? 0 : 1;
}