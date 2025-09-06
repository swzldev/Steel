#pragma once

#include <string>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <unordered_map>
#include <memory>

#include "../parser/types/types.h"
#include "../parser/types/custom_types.h"

class runtime_value {
public:
    runtime_value()
        : type(DT_UNKNOWN), value("") {
    }
    runtime_value(const data_type& t, const std::string& v)
        : type(t), value(v) {
    }
    runtime_value(const data_type& t)
        : type(t), value("") {
    }

    data_type type;
    std::string value;
    std::unordered_map<std::string, std::shared_ptr<runtime_value>> members;

    inline bool is_int()       const { return type.primitive == DT_I32; }
    inline bool is_float()     const { return type.primitive == DT_FLOAT; }
    inline bool is_char()      const { return type.primitive == DT_CHAR; }
    inline bool is_string()    const { return type.primitive == DT_STRING; }
    inline bool is_bool()      const { return type.primitive == DT_BOOL; }
    inline bool is_void()      const { return type.primitive == DT_VOID; }
    inline bool is_unknown()   const { return type.primitive == DT_UNKNOWN; }
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
        return value;
    }
    bool as_bool() const {
        if (!is_bool()) throw std::runtime_error("runtime_value: not a bool");
        return value == "true" || value == "1";
    }

    void set_member(const std::string& name, std::shared_ptr<runtime_value> val) {
        members[name] = val;
    }

    std::shared_ptr<runtime_value> get_member(const std::string& name) const {
        auto it = members.find(name);
        if (it != members.end()) return it->second;
        return nullptr;
    }

    bool operator==(const runtime_value& other) const {
        return type.primitive == other.type.primitive && value == other.value;
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
};