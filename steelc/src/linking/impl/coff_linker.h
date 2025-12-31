#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <filesystem>

#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <linking/icode_linker.h>
#include <linking/sys/system_linker.h>

class coff_linker : public icode_linker {
public:
	virtual bool can_emit_executable() override { return true; }
	virtual bool can_emit_static_lib() override { return true; }
	virtual bool can_emit_shared_lib() override { return true; }

	virtual link_result link(const std::vector<std::string>& to_link, const codegen_config& cfg);

private:
	std::filesystem::path find_vcvarsall();
	std::string get_host_target_string(const std::string& target_arch);
	std::unordered_map<std::string, std::vector<std::string>> capture_vcvarsall(const std::string& target_arch);

	std::string get_host_arch();
};