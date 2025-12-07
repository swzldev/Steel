#include "module_entity.h"

#include "../modules/module_info.h"

std::shared_ptr<module_entity> module_entity::make(std::shared_ptr<module_info> mod_info) {
	return std::shared_ptr<module_entity>(new module_entity(mod_info));
}

std::string module_entity::name() const {
	return mod_info->name;
}
std::string module_entity::full_name() const {
	return mod_info->full_name();
}

std::shared_ptr<module_entity> module_entity::as_module() {
	return std::static_pointer_cast<module_entity>(shared_from_this());
}
