#pragma once

#include <vector>
#include <string>

#include "source_file.h"

struct dependency {
	std::string name;
	std::string path;
};

class stproj_file {
public:
	static stproj_file load(std::string path);
	void save(const std::string& path);

	std::string project_name;
	std::string project_version;
	std::string project_type;
	std::vector<source_file> sources;
	std::vector<dependency> dependencies;
	
private:
	stproj_file() = default;
};