#pragma once

#include <map>

#include "data_type.h"

class type_declaration;

enum custom_type_kind {
	CT_CLASS,
	CT_STRUCT,
	CT_INTERFACE,
};

class custom_type : public data_type {
public:
	// retrieves a custom type, or creates one if it doesnt exist
	static std::shared_ptr<custom_type> get(const std::string& identifier);

	bool operator==(const data_type_ptr other) const override;

	int size_of() const override;
	std::string name() const override {
		return identifier;
	}

	bool is_indexable() const override {
		return false;
	}

	unsigned int type_id() const {
		return id;
	}

	std::shared_ptr<const type_declaration> declaration;

private:
	custom_type(const std::string& identifier)
		: data_type(DT_CUSTOM), declaration(nullptr), identifier(identifier) {
		static unsigned int next_id = 0;
		id = next_id++;
	}

	std::string identifier;
	unsigned int id;

	static std::map<std::string, std::shared_ptr<custom_type>> type_map;
};