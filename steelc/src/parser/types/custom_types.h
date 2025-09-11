#pragma once

#include "types.h"

class type_declaration;

enum custom_type_type {
	CT_CLASS,
	CT_STRUCT,
	CT_INTERFACE,
};

class custom_type : public data_type {
public:
	custom_type(std::shared_ptr<const type_declaration> declaration, std::string identifier)
		: declaration(declaration), identifier(identifier) {
	}
	custom_type(std::string identifier)
		: declaration(nullptr), identifier(identifier) {
	}

	bool operator==(const data_type& other) const override {
		if (auto custom = dynamic_cast<const custom_type*>(&other)) {
			// compare typename
			return identifier == custom->identifier;
		}
		return false;
	}
	bool operator!=(const data_type& other) const {
		return !(*this == other);
	}

	int size_of() const override;

	bool is_primitive() const override {
		return false;
	}
	bool is_indexable() const override {
		return false;
	}

	std::string type_name() const override {
		return identifier;
	}

	std::shared_ptr<const type_declaration> declaration;
	std::string identifier;
};