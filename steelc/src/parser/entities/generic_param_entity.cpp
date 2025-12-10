#include "generic_param_entity.h"

#include <string>
#include <memory>

#include "../ast/generics/generic_parameter.h"

std::shared_ptr<generic_param_entity> generic_param_entity::make(std::shared_ptr<generic_parameter> gp) {
    return std::shared_ptr<generic_param_entity>(new generic_param_entity(gp));
}

std::string generic_param_entity::name() const {
    return declaration->identifier;
}
std::string generic_param_entity::full_name() const {
    // may change this in the future
    return declaration->identifier;
}

std::shared_ptr<generic_param_entity> generic_param_entity::as_generic_param() {
    return shared_from_this();
}
