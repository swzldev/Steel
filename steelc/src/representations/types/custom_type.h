#pragma once

#include <map>

#include <representations/types/data_type.h>
#include <ast/ast_fwd.h>

typedef unsigned int type_id;

enum custom_type_type {
	CT_CLASS,
	CT_STRUCT,
	CT_INTERFACE,
};

class custom_type : public data_type {
public:
	// retrieves a custom type, or creates one if it doesnt exist
	static std::shared_ptr<custom_type> get(const std::string& identifier, std::vector<type_ptr> generics = {});

	// this is public for make_shared access and to prevent errors. DO NOT USE IT
	custom_type(const std::string& identifier)
		: data_type(DT_CUSTOM), declaration(nullptr), identifier(identifier) {
		static type_id next_id = 0;
		id = next_id++;
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

	type_id tid() const {
		return id;
	}

	std::shared_ptr<type_declaration> declaration;

private:
	std::string identifier;
	type_id id;

	static std::map<std::string, std::shared_ptr<custom_type>> type_map;
	static std::map<std::string, std::vector<std::shared_ptr<custom_type>>> generic_type_map;
};