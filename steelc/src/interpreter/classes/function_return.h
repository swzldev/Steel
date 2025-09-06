#pragma once

#include "runtime_value.h"

class function_return {
public:
	function_return(const runtime_value& v) : value(v) {}

	runtime_value value;
};