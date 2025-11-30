#pragma once

#include <string>
#include <memory>

#include "../../parser/ast/compilation_unit.h"

class ir_holder {
public:
	ir_holder() = default;

	std::string ir;
	std::shared_ptr<compilation_unit> owning_unit;
};