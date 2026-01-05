#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>

#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <linking/icode_linker.h>
#include <linking/link_result.h>
#include <linking/link_data.h>
#include <building/cache/vars_file.h>
#include <sys/platform.h>

class coff_linker : public icode_linker {
public:
	virtual bool can_emit_executable() override { return true; }
	virtual bool can_emit_static_lib() override { return true; }
	virtual bool can_emit_shared_lib() override { return true; }

	virtual link_result link(const link_data& data) override;

private:
	std::vector<std::string> get_stdlib_paths(platform_arch target, vars_file& vf);

	std::filesystem::path find_vcvarsall();
	std::string get_host_target_string(platform_arch target);
	std::unordered_map<std::string, std::vector<std::string>> capture_vcvarsall(platform_arch target);

	std::string get_arch_string(platform_arch arch);

	inline platform_arch get_host_arch() {
		return platform::host_platform().arch;
	}
};