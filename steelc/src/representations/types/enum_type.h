#pragma once

#include <map>

#include <representations/types/data_type.h>

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
		if (!is_enum_option) {
			return identifier;
		}
		return identifier + "." + option_identifier;
	}

	std::shared_ptr<data_type> clone() const override;

	std::shared_ptr<enum_declaration> declaration;
	// whether this is the enum declaration itself or an option within the enum
	bool is_enum_option = false;
	std::string option_identifier;

private:
	std::string identifier;
};