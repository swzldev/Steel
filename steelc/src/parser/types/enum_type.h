#pragma once

#include <map>

#include "data_type.h"

class enum_declaration;

class enum_type : public data_type {
public:
	enum_type(const std::string& identifier, std::shared_ptr<enum_declaration> declaration)
		: data_type(DT_ENUM), declaration(declaration), identifier(identifier) {
	}

	bool operator==(const type_ptr& other) const override;

	int size_of() const override;

	bool is_primitive() const override {
		return false;
	}
	bool is_indexable() const override {
		return false;
	}

	std::string name() const override {
		return identifier;
	}

	std::shared_ptr<data_type> clone() const override;

	std::shared_ptr<enum_declaration> declaration;

private:
	std::string identifier;
};