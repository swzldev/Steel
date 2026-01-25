#include "coff_linker.h"

#include <vector>
#include <string>
#include <filesystem>

#include <output/output.h>
#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <linking/link_result.h>
#include <linking/sys/system_linker.h>
#include <linking/link_data.h>
#include <linking/link_config.h>
#include <utils/path_utils.h>
#include <utils/command_builder.h>
#include <sys/host_defs.h>
#include <sys/platform.h>
#include <sys/shell.h>

namespace fs = std::filesystem;

link_result coff_linker::link(const link_data& data) {
#if !defined(STEELC_PLATFORM_WINDOWS)
	return link_error{
		.message = "The COFF linker is only supported on Windows hosts"
	};
#else

	const target_triple& target = data.cfg.target;

	platform_abi abi = target.abi();
	if (abi == platform_abi::UNKNOWN && target.abi_explicit()) {
		return link_error{
			.message = "Invalid abi specified for target triple: " + target.string
		};
	}
	else if (abi == platform_abi::UNKNOWN) {
		output::verbose("No abi specified. Using default (msvc).\n");
		abi = platform_abi::MSVC;
	}
	else if (abi != platform_abi::MSVC) {
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

	command_builder cb(sys_linker->executable_path);

	// output file
	fs::path out_path = fs::path(data.cfg.output_dir) / (data.cfg.output_name + ".exe");
	out_path = path_utils::normalize(fs::weakly_canonical(out_path));
	cb << "/OUT:" + out_path.string();

	// add inputs
	for (const auto& obj_path : data.object_files) {
		cb << obj_path;
	}

	// machine
	platform_arch arch = target.arch();
	std::string arch_str;
	if (arch == platform_arch::UNKNOWN && target.arch_explicit()) {
		return link_error{
			.message = "Invalid architecture specified for target triple: " + target.string
		};
	}
	else if (arch == platform_arch::UNKNOWN) {
		platform_arch host_arch = get_host_arch();
		arch_str = get_arch_string(host_arch);
		output::verbose("No architecture specified. Using host ({}).\n", console_colors::DIM, arch_str, "\n");
		arch = host_arch;
	}
	else {
		arch_str = get_arch_string(arch);
	}
	cb << "/MACHINE:" + arch_str;
	

	// lib paths
	auto lib_paths = get_stdlib_paths(arch, data.cached_vars);
	if (!lib_paths.empty()) {
		for (const auto& lp : lib_paths) {
			cb << "/LIBPATH:" + lp;
		}
	}
	else {
		output::print("Warn: Could not capture standard library paths, linking will likely fail.\n");
	}

	// subsystem (always console for now)
	cb << "/SUBSYSTEM:CONSOLE";

	// for now link in release using static crt
	cb << "libcmt.lib" << "libvcruntime.lib" << "libucrt.lib" << "kernel32.lib";

	// suppress logo
	cb << "/NOLOGO";

	// execute linker
	int ec = shell::exec("\"" + cb.build() + "\"");
	if (ec != 0) {
		return link_error{
			.message = "Linker failed with exit code: " + std::to_string(ec)
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

#endif // STEELC_PLATFORM_WINDOWS
}

std::vector<std::string> coff_linker::get_stdlib_paths(platform_arch target, vars_file& vf) {
	std::string target_str = get_arch_string(target);
	std::string key = "coff_stdlib_dirs_" + target_str;
	std::vector<std::string> paths;

	// check if cached
	if (vf.get_var(key, paths)) {
		// verify paths are still valid
		bool all_exist = true;
		for (const auto& p : paths) {
			if (!fs::exists(p) || !fs::is_directory(p)) {
				all_exist = false;
				break;
			}
		}

		if (all_exist) return paths;
		// not all exist, we need to recapture
	}

	// load using vcvarsall
	auto vc_env = capture_vcvarsall(target);
	if (!vc_env.empty()) {
		auto it = vc_env.find("LIB");
		if (it != vc_env.end()) {
			for (const auto& lib_path : it->second) {
				paths.push_back(lib_path);
			}
		}
	}
	// cache
	vf.set_var(key, paths);

	return paths;
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
std::string coff_linker::get_host_target_string(platform_arch target)
{
	platform_arch host = get_host_arch();
	if (host == target) {
		// native build, dont respecify target
		return get_arch_string(host);
	}

	std::string host_str = get_arch_string(host);
	std::string target_str = get_arch_string(target);

	return host_str + "_" + target_str;
}
std::unordered_map<std::string, std::vector<std::string>> coff_linker::capture_vcvarsall(platform_arch target) {
	fs::path vcvarsall_path = find_vcvarsall();
	if (!fs::exists(vcvarsall_path)) {
		return {}; // failed to find
	}

	std::string host_target = get_host_target_string(target);

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

std::string coff_linker::get_arch_string(platform_arch arch) {
	switch (arch) {
	case platform_arch::X86:
		return "x86";
	case platform_arch::X64:
		return "x64";
	case platform_arch::ARM:
		return "arm";
	case platform_arch::ARM64:
		return "arm64";
	default:
		throw std::runtime_error("Unknown platform architecture");
	}
}
