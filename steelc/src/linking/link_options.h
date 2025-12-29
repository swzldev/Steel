#pragma once

class link_options {
public:
	enum class os {
		windows,
		linux,
		osx,
	};
	enum class output_type {
		executable,
		static_library,
		shared_library,
	};

public:
	os target_os;
	output_type target_type;
};