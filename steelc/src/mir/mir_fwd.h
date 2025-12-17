#pragma once

// forward declarations for all mir types
//
// not including:
//  - enums such as mir_instr_kind
//  - using declarations such as mir_operand

struct mir_module;
class mir_function;
class mir_block;
struct mir_instr;
class mir_value;
struct mir_type;
struct mir_const_int;
struct mir_const_float;
struct mir_func_ref;
struct mir_field_ref;
