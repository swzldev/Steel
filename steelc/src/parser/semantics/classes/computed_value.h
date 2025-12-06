#pragma once

#include <string>

#include "../../types/data_type.h"
#include "../../parser_utils.h"

class computed_value {
public:
    computed_value(const type_ptr t, const std::string& v)
        : type(t), value(v) {
    }
    computed_value(bool v)
        : type(to_data_type(DT_BOOL)), value(v ? "true" : "false") {
    }

    inline bool is_int()            const { return type->is_primitive() && type->is_integer(); }
    inline bool is_float()          const { return type->is_primitive() && type->is_floating_point(); }
    inline bool is_char()           const { return type->is_primitive() && type->is_character(); }
    inline bool is_string()         const { return type->is_primitive() && type->is_text(); }
    inline bool is_bool()           const { return type->is_primitive() && type->primitive == DT_BOOL; }
    inline bool is_pointer()        const { return type->is_pointer(); }
    inline bool is_unknown()        const { return type->is_unknown(); }
    inline bool is_number()         const { return is_int() || is_float(); }

    // these functions are not type-safe! ALWAYS check the type before using!!
    inline int as_int()             const { return std::stoi(value); }
    inline double as_float()        const { return std::stod(value); }
    inline char as_char()           const { return value[0]; }
    inline std::string as_string()  const { return value; }
    inline bool as_bool()           const { return value == "true"; }

    bool operator==(const computed_value& other) const;
    bool operator!=(const computed_value& other) const;
    bool operator<(const computed_value& other) const;
    bool operator<=(const computed_value& other) const;
    bool operator>(const computed_value& other) const;
    bool operator>=(const computed_value& other) const;

    computed_value operator+(const computed_value& other);
    computed_value operator-(const computed_value& other);

private:
    type_ptr type;
    std::string value;
};