#pragma once

#include <variant>

// forward declarations for all mir types

struct mir_module;
class mir_function;
class mir_block;
struct mir_instr;
class mir_value;
struct mir_type;

enum class mir_instr_opcode;

struct mir_const_int;
struct mir_const_float;
struct mir_string_imm;
struct mir_func_ref;
struct mir_field_ref;
