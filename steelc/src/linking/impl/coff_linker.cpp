#include "coff_linker.h"

#include <vector>
#include <string>
#include <filesystem>

#include <output/output.h>
#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <codegen/target/target_triple.h>
#include <linking/link_result.h>
#include <linking/sys/system_linker.h>
#include <steelc/platform_constants.h>
#include <utils/path_utils.h>
#include <utils/shell.h>

namespace fs = std::filesystem;

link_result coff_linker::link(const std::vector<std::string>& to_link, const codegen_config& cfg) {
#if !defined(STEELC_PLATFORM_WINDOWS)
	return link_error{
		.message = "The COFF linker is only supported on Windows hosts"
	};
#endif

	target_triple tri(cfg.target_triple);

	std::string abi = tri.abi;
	if (abi.empty()) {
		output::verbose("No abi specified. Using default: msvc\n");
		abi = "msvc";
	}
	else if (abi != "msvc") {
		return link_error{
			.message = "The COFF linker only supports compiling for the MSVC abi"
		};
	}

	// only 2 for now (since we only support msvc)
	std::vector<std::string> linker_candidates = {
		"lld-link",
		"link"
	};

	auto sys_linker = system_linker::probe(linker_candidates);
	if (!sys_linker) {
		return link_error{
			.message = "No suitable COFF linker found on system (tried lld-link, link). Ensure LLVM or Visual Studio is installed and try again"
		};
	}

	std::vector<std::string> args;

	// output file
	fs::path out_path = fs::path(cfg.output_dir) / (cfg.output_name + ".exe");
	args.push_back("/OUT:\"" + fs::weakly_canonical(out_path).string() + "\"");

	// add inputs
	for (const auto& obj_path : to_link) {
		args.push_back("\"" + obj_path + "\"");
	}

	// machine
	std::string arch = tri.arch;
	if (arch.empty()) {
		std::string host_arch = get_host_arch();
		output::verbose("No architecture specified. Using host: {}\n", console_colors::DIM, host_arch);
		arch = host_arch;
	}
	if (arch == "x86_64" || arch == "amd64") {
		args.push_back("/MACHINE:X64");
	} else if (arch == "i686" || arch == "x86") {
		args.push_back("/MACHINE:X86");
	} else if (arch == "arm" || arch == "armv7" || arch == "armv7l") {
		args.push_back("/MACHINE:ARM");
	} else if (arch == "arm64" || arch == "aarch64") {
		args.push_back("/MACHINE:ARM64");
	}

	// lib paths
	bool found_lib_paths = false;
	auto vc_env = capture_vcvarsall(arch);
	if (!vc_env.empty()) {
		auto it = vc_env.find("LIB");
		if (it != vc_env.end()) {
			found_lib_paths = true;
			for (const auto& lib_path : it->second) {
				args.push_back("/LIBPATH:\"" + lib_path + "\"");
			}
		}
	}
	if (!found_lib_paths) {
		output::print("Warn: Could not capture standard library paths, linking will likely fail.\n");
	}

	// subsystem (always console for now)
	args.push_back("/SUBSYSTEM:CONSOLE");

	// for now link in release using dynamic crt
	args.push_back("ucrt.lib");
	args.push_back("vcruntime.lib");

	// suppress logo
	args.push_back("/NOLOGO");

	// execute linker
	int exit_code = sys_linker->execute(args);
	if (exit_code != 0) {
		return link_error{
			.message = "Linker failed with exit code: " + std::to_string(exit_code)
		};
	}

	if (!fs::exists(out_path) || !fs::is_regular_file(out_path)) {
		return link_error{
			.message = "Linker failed to generate executable"
		};
	}

	link_result result;
	result.final_executable_path = out_path.string();
	return result;
}

std::filesystem::path coff_linker::find_vcvarsall() {
	static const std::string vcvarsall_relative = "VC/Auxiliary/Build/vcvarsall.bat";

	// try VSINSTALLDIR
	if (const char* vsdir = std::getenv("VSINSTALLDIR")) {
		auto p = fs::path(vsdir) / vcvarsall_relative;
		if (fs::exists(p)) return p;
	}

	// try vswhere.exe
	fs::path pf86 = std::getenv("ProgramFiles(x86)");
	fs::path vswhere = path_utils::normalize(pf86 / "Microsoft Visual Studio/Installer/vswhere.exe");
	if (fs::exists(vswhere)) {
		std::string command = "\"" + vswhere.string() + "\" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath";
		std::string output = shell::exec_piped(command);
		std::istringstream iss(output);
		std::string line;
		while (std::getline(iss, line)) {
			auto p = fs::path(line) / vcvarsall_relative;
			if (fs::exists(p)) return path_utils::normalize(p);
		}
	}

	return fs::path();
}
std::string coff_linker::get_host_target_string(const std::string& target_arch)
{
	std::string host;

#if defined(STEELC_ARCH_X64)
	host = "x64";
#elif defined(STEELC_ARCH_X86)
	host = "x86";
#elif defined(STEELC_ARCH_ARM64)
	host = "arm64";
#elif defined(STEELC_ARCH_ARM)
	host = "arm";
#else
	throw std::runtime_error("Unknown host architecture");
#endif

	std::string target;

	if (target_arch == "x86_64" || target_arch == "amd64" || target_arch == "x64") {
		target = "x64";
	}
	else if (target_arch == "i686" || target_arch == "x86") {
		target = "x86";
	}
	else if (target_arch == "arm64" || target_arch == "aarch64") {
		target = "arm64";
	}
	else if (target_arch == "arm" || target_arch == "armv7" || target_arch == "armv7l") {
		target = "arm";
	}
	else {
		throw std::runtime_error("Unsupported target architecture for vcvarsall: " + target_arch);
	}

	// native build, dont respecify target
	if (host == target) {
		return host;
	}

	return host + "_" + target;
}
std::unordered_map<std::string, std::vector<std::string>> coff_linker::capture_vcvarsall(const std::string& target_arch) {
	fs::path vcvarsall_path = find_vcvarsall();
	if (!fs::exists(vcvarsall_path)) {
		return {}; // failed to find
	}

	std::string host_target = get_host_target_string(target_arch);

	std::string command = "call \"" + vcvarsall_path.string() + "\" " + host_target + " > nul 2>&1 && set";
	std::string output = shell::exec_piped(command);
	std::unordered_map<std::string, std::vector<std::string>> env_vars;
	std::istringstream iss(output);
	std::string line;

	// parse output lines of form KEY=VALUE
	while (std::getline(iss, line)) {
		size_t eq_pos = line.find('=');
		if (eq_pos != std::string::npos) {
			std::string key = line.substr(0, eq_pos);
			std::string value = line.substr(eq_pos + 1);

			// seperate multi path entries (with ';')
			std::vector<std::string> values;
			std::istringstream vss(value);
			std::string path_entry;
			while (std::getline(vss, path_entry, ';')) {
				values.push_back(path_entry);
			}
			env_vars[key] = values;
		}
	}

	return env_vars;
}

std::string coff_linker::get_host_arch() {
#if defined(STEELC_ARCH_X64)
	return "x86_64";
#elif defined(STEELC_ARCH_X86)
	return "i686";
#elif defined(STEELC_ARCH_ARM64)
	return "aarch64";
#elif defined(STEELC_ARCH_ARM)
	return "arm";
#endif
}
