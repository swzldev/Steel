#include <memory>

#include "entity.h"

struct module_info;

class module_entity : public entity, public std::enable_shared_from_this<module_entity> {
public:
	static std::shared_ptr<module_entity> make(std::shared_ptr<module_info> mod_info);

	std::string name() const override;
	std::string full_name() const override;

	std::shared_ptr<module_entity> as_module() override;

	std::shared_ptr<module_info> mod_info;

private:
	module_entity(std::shared_ptr<module_info> mod_info)
		: entity(ENTITY_MODULE), mod_info(mod_info) {
	}
};