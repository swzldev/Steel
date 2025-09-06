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
		assign_id();
	}
	custom_type(std::string identifier)
		: declaration(nullptr), identifier(identifier) {
		// this is used for types that we know exist but
		// are not at the stage where the declaration is matched
		assign_id();
	}

	bool operator==(const data_type& other) const override {
		if (auto custom = dynamic_cast<const custom_type*>(&other)) {
			// compare typeid
			return id == custom->type_id();
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

	unsigned int type_id() const {
		return id;
	}

	std::shared_ptr<const type_declaration> declaration;
	std::string identifier;

private:
	inline void assign_id() {
		static unsigned int next_id = 100;
		id = next_id++;
	}

	unsigned int id;
};