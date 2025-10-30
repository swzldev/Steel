#pragma once

#include <string>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <unordered_map>
#include <memory>

#include "../parser/ast/ast_fwd.h"
#include "../parser/types/data_type.h"
#include "../parser/types/custom_type.h"
#include "../parser/types/type_utils.h"

class runtime_value {
public:
    runtime_value()
        : type(data_type::UNKNOWN), value("") {
    }
    runtime_value(const type_ptr t, const std::string& v)
        : type(t), value(v) {
    }
    runtime_value(const type_ptr t)
        : type(t), value(get_default_value(t)) {
    }
    runtime_value(std::shared_ptr<runtime_value> pointee)
        : type(make_pointer(pointee->type)), value(std::to_string((long long)pointee.get())), pointee(pointee) {
    }
    runtime_value(type_ptr elem_type, const std::vector<std::shared_ptr<runtime_value>>& elems)
        : type(make_array(elem_type)), value(""), elements(elems) {
    }

    type_ptr type;
    std::string value;
    std::vector<std::shared_ptr<runtime_value>> elements;
    std::shared_ptr<runtime_value> pointee;
    std::unordered_map<std::string, std::shared_ptr<runtime_value>> members;
    std::unordered_map<std::string, std::shared_ptr<function_declaration>> vftable;

    inline bool is_int()       const { return type->is_primitive() && type->primitive == DT_I32; }
    inline bool is_float()     const { return type->is_primitive() && type->primitive == DT_FLOAT; }
    inline bool is_char()      const { return type->is_primitive() && type->primitive == DT_CHAR; }
    inline bool is_string()    const { return type->is_primitive() && type->primitive == DT_STRING; }
    inline bool is_bool()      const { return type->is_primitive() && type->primitive == DT_BOOL; }
    inline bool is_void()      const { return type->is_primitive() && type->primitive == DT_VOID; }
    inline bool is_pointer()   const { return type->is_pointer(); }
    inline bool is_array()     const { return type->is_array(); }
	inline bool is_enum()      const { return type->is_enum(); }
    inline bool is_unknown()   const { return type->is_unknown(); }
    inline bool is_number()    const { return is_int() || is_float(); }

    int as_int() const {
        if (!is_int()) throw std::runtime_error("runtime_value: not an int");
        return std::stoi(value);
    }
    double as_float() const {
        if (!is_float() && !is_int()) throw std::runtime_error("runtime_value: not a float or int");
        return std::stod(value);
    }
    char as_char() const {
        if (!is_char()) throw std::runtime_error("runtime_value: not a char");
        if (value.empty()) throw std::runtime_error("runtime_value: char value is empty");
        return value[0];
    }
    std::string as_string() const {
        if (is_string()) return value;
        if (is_char()) return std::string(1, as_char());
        if (is_int() || is_float() || is_bool()) return value;
        if (is_pointer()) {
            std::ostringstream oss;
            oss << "0x" << std::hex << std::stoll(value);
            return oss.str();
        }
        return value;
    }
    bool as_bool() const {
        if (!is_bool()) throw std::runtime_error("runtime_value: not a bool");
        return value == "true" || value == "1";
    }
    std::vector<std::shared_ptr<runtime_value>> as_array() const {
        if (!is_array()) throw std::runtime_error("runtime_value: not an array");
        return elements;
    }

    inline void set_member(const std::string& name, std::shared_ptr<runtime_value> val) {
        members[name] = val;
    }
    inline std::shared_ptr<runtime_value> get_member(const std::string& name) const {
        auto it = members.find(name);
        if (it != members.end()) return it->second;
        return nullptr;
    }

    bool operator==(const runtime_value& other) const {
        return type == other.type && value == other.value;
    }
    bool operator!=(const runtime_value& other) const {
        return !(*this == other);
    }
    bool operator<(const runtime_value& other) const {
        if (is_int() && other.is_int()) return as_int() < other.as_int();
        if (is_float() && other.is_float()) return as_float() < other.as_float();
        if (is_char() && other.is_char()) return as_char() < other.as_char();
        if (is_string() && other.is_string()) return value < other.value;
        throw std::runtime_error("runtime_value: unsupported types for < comparison");
    }
    bool operator<=(const runtime_value& other) const {
        return *this < other || *this == other;
    }
    bool operator>(const runtime_value& other) const {
        return !(*this <= other);
    }
    bool operator>=(const runtime_value& other) const {
        return !(*this < other);
    }

private:
    std::string get_default_value(type_ptr type) {
		if (type->is_primitive()) {
			switch (type->primitive) {
			case DT_I16:
			case DT_I32:
			case DT_I64:
				return "0";
			case DT_FLOAT:
			case DT_DOUBLE:
				return "0.0";
			case DT_CHAR:
				return "\0";
			case DT_STRING:
				return "";
			case DT_BOOL:
				return "false";
			case DT_VOID:
				return "";
			default:
				return "";
			}
		}
		else if (type->is_pointer() || type->is_array()) {
			return "0";
		}
		return "";
    }
};

using val_ptr = std::shared_ptr<runtime_value>;