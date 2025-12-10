#pragma once

#include <memory>

// entity forward declarations
class entity;
class variable_entity;
class function_entity;
class type_entity;
class module_entity;
class generic_param_entity;

using entity_ptr = std::shared_ptr<entity>;