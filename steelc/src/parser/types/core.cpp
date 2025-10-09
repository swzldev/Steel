#include "core.h"

std::vector<conversion_declaration> get_core_conversions(primitive_type from) {
    static std::vector<conversion_declaration> builtin_conversions = {
        {DT_I32, DT_FLOAT, true},
        {DT_FLOAT, DT_I32, true},
        {DT_I32, DT_CHAR},
        {DT_CHAR, DT_I32},
        {DT_CHAR, DT_STRING},
        {DT_BOOL, DT_I32},
        {DT_I32, DT_BOOL},
        {DT_POINTER, DT_I32},
    };
    if (!from) {
        return builtin_conversions;
    }
    std::vector<conversion_declaration> filtered_conversions;
    for (const auto& conv : builtin_conversions) {
        if (conv.from->primitive == from) {
            filtered_conversions.push_back(conv);
        }
    }
    return filtered_conversions;
}
std::vector<operator_declaration> get_core_operators(token_type filter) {
    static std::vector<operator_declaration> builtin_ops = {
        // addition
        {DT_STRING,    DT_STRING,    TT_PLUS,        DT_STRING},
        {DT_STRING,    DT_STRING,    TT_PLUS,        DT_CHAR},
        {DT_STRING,    DT_CHAR,      TT_PLUS,        DT_STRING},
        {DT_CHAR,      DT_CHAR,      TT_PLUS,        DT_I32},
        {DT_CHAR,      DT_I32,       TT_PLUS,        DT_CHAR},
        {DT_CHAR,      DT_CHAR,      TT_PLUS,        DT_CHAR},
        {DT_I32,       DT_I32,       TT_PLUS,        DT_I32},
        {DT_FLOAT,     DT_FLOAT,     TT_PLUS,        DT_FLOAT},
        {DT_FLOAT,     DT_I32,       TT_PLUS,        DT_FLOAT},
        {DT_FLOAT,     DT_FLOAT,     TT_PLUS,        DT_I32},
        {DT_I32,       DT_FLOAT,     TT_PLUS,        DT_FLOAT},

        // subtraction
        {DT_I32,       DT_I32,       TT_MINUS,   DT_I32},
        {DT_FLOAT,     DT_FLOAT,     TT_MINUS,   DT_FLOAT},
        {DT_FLOAT,     DT_I32,       TT_MINUS,   DT_FLOAT},
        {DT_FLOAT,     DT_FLOAT,     TT_MINUS,   DT_I32},
        {DT_I32,       DT_FLOAT,     TT_MINUS,   DT_FLOAT},
        {DT_CHAR,      DT_CHAR,      TT_MINUS,   DT_CHAR},
        {DT_CHAR,      DT_CHAR,      TT_MINUS,   DT_I32},

        // multiplication
        {DT_STRING,    DT_STRING,    TT_ASTERISK,   DT_I32},
        {DT_STRING,    DT_CHAR,      TT_ASTERISK,   DT_I32},
        {DT_STRING,    DT_I32,       TT_ASTERISK,   DT_STRING},
        {DT_STRING,    DT_I32,       TT_ASTERISK,   DT_CHAR},
        {DT_STRING,    DT_CHAR,      TT_ASTERISK,   DT_STRING},
        {DT_CHAR,      DT_I32,       TT_ASTERISK,   DT_CHAR},
        {DT_I32,       DT_STRING,    TT_ASTERISK,   DT_STRING},
        {DT_I32,       DT_CHAR,      TT_ASTERISK,   DT_STRING},
        {DT_I32,       DT_I32,       TT_ASTERISK,   DT_I32},
        {DT_FLOAT,     DT_FLOAT,     TT_ASTERISK,   DT_FLOAT},
        {DT_FLOAT,     DT_I32,       TT_ASTERISK,   DT_FLOAT},
        {DT_FLOAT,     DT_FLOAT,     TT_ASTERISK,   DT_I32},
        {DT_I32,       DT_FLOAT,     TT_ASTERISK,   DT_FLOAT},

        // division
        {DT_I32,       DT_I32,       TT_FSLASH,     DT_I32},
        {DT_FLOAT,     DT_FLOAT,     TT_FSLASH,     DT_FLOAT},
        {DT_FLOAT,     DT_I32,       TT_FSLASH,     DT_FLOAT},
        {DT_FLOAT,     DT_FLOAT,     TT_FSLASH,     DT_I32},
        {DT_I32,       DT_FLOAT,     TT_FSLASH,     DT_FLOAT},

        // modulo
        {DT_I32,       DT_I32,       TT_PERCENT,     DT_I32},
        {DT_FLOAT,     DT_FLOAT,     TT_PERCENT,     DT_FLOAT},
        {DT_FLOAT,     DT_I32,       TT_PERCENT,     DT_FLOAT},
        {DT_FLOAT,     DT_FLOAT,     TT_PERCENT,     DT_I32},
        {DT_I32,       DT_FLOAT,     TT_PERCENT,     DT_FLOAT},
        {DT_CHAR,      DT_CHAR,      TT_PERCENT,     DT_I32},
        {DT_I32,       DT_I32,       TT_PERCENT,     DT_CHAR},

        // comparison
        {DT_BOOL,      DT_I32,       TT_LESS,       DT_I32},
        {DT_BOOL,      DT_I32,       TT_LESS_EQ,    DT_I32},
        {DT_BOOL,      DT_I32,       TT_GREATER,    DT_I32},
        {DT_BOOL,      DT_I32,       TT_GREATER_EQ, DT_I32},
        {DT_BOOL,      DT_FLOAT,     TT_LESS,       DT_FLOAT},
        {DT_BOOL,      DT_FLOAT,     TT_LESS_EQ,    DT_FLOAT},
        {DT_BOOL,      DT_FLOAT,     TT_GREATER,    DT_FLOAT},
        {DT_BOOL,      DT_FLOAT,     TT_GREATER_EQ, DT_FLOAT},

        // logical
        {DT_BOOL,      DT_BOOL,      TT_AND,        DT_BOOL},
        {DT_BOOL,      DT_BOOL,      TT_OR,         DT_BOOL},

        // equality
        {DT_BOOL,      DT_I32,       TT_EQUAL,      DT_I32},
        {DT_BOOL,      DT_I32,       TT_NOT_EQUAL,  DT_I32},
        {DT_BOOL,      DT_FLOAT,     TT_EQUAL,      DT_FLOAT},
        {DT_BOOL,      DT_FLOAT,     TT_NOT_EQUAL,  DT_FLOAT},
        {DT_BOOL,      DT_BOOL,      TT_EQUAL,      DT_BOOL},
        {DT_BOOL,      DT_BOOL,      TT_NOT_EQUAL,  DT_BOOL},
        {DT_BOOL,      DT_CHAR,      TT_EQUAL,      DT_CHAR},
        {DT_BOOL,      DT_CHAR,      TT_NOT_EQUAL,  DT_CHAR},
        {DT_BOOL,      DT_STRING,    TT_EQUAL,      DT_STRING},
        {DT_BOOL,      DT_STRING,    TT_NOT_EQUAL,  DT_STRING},
    };
    if (filter == TT_UNKNOWN) {
        return builtin_ops;
    }
    std::vector<operator_declaration> filtered_ops;
    for (const auto& op : builtin_ops) {
        if (op.oparator == filter) {
            filtered_ops.push_back(op);
        }
    }
    return filtered_ops;
}