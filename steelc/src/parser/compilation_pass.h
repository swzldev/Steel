#pragma once

#include <string>
#include <vector>
#include <cstdarg>
#include <cstdio>
#include <memory>

#include "../lexer/token.h"
#include "../error/error_catalog.h"

#define ERROR_TOKEN(code, tk, ...)  report_error_token(code, tk, __VA_ARGS__)
#define ERROR(code, pos, ...) report_error(code, pos, __VA_ARGS__)
#define WARN_TOKEN(code, tk, ...)   report_warning_token(code, tk, __VA_ARGS__)
#define WARN(code, pos, ...)  report_warning(code, pos, __VA_ARGS__)

class compilation_pass {
public:
	compilation_pass(std::shared_ptr<compilation_unit> unit)
		: pass_unit(unit) {
	}

    std::vector<error> errors;
    std::shared_ptr<compilation_unit> pass_unit;

    inline bool has_errors() const {
        return !errors.empty();
    }

    inline std::vector<error> get_errors() const {
        return errors;
    }

protected:
    void report_error_token(error_code code_enum, token tk, ...) {
        __debugbreak();
        va_list args;
        va_start(args, tk);
        add_error(code_enum, tk.pos.line, tk.pos.column, args);
        va_end(args);
    }
    void report_error(error_code code_enum, position pos, ...) {
        va_list args;
        va_start(args, pos);
        add_error(code_enum, pos.line, pos.column, args);
        va_end(args);
    }

    void report_warning_token(warning_code code_enum, token tk, ...) {
        va_list args;
        va_start(args, tk);
        add_warning(code_enum, tk.pos.line, tk.pos.column, args);
        va_end(args);
    }
    void report_warning(warning_code code_enum, position pos, ...) {
        va_list args;
        va_start(args, pos);
        add_warning(code_enum, pos.line, pos.column, args);
        va_end(args);
    }

private:
    void add_error(error_code code_enum, size_t line, size_t column, va_list args) {
        error_info info = error_catalog::get_error_info(code_enum);
        info.message = vformat(info.message, args);
        errors.push_back({ info, { line, column }, ERR_ERROR, pass_unit });
    }

    void add_warning(warning_code code_enum, size_t line, size_t column, va_list args) {
        error_info info = error_catalog::get_warning_info(code_enum);
        info.message = vformat(info.message, args);
        errors.push_back({ info, { line, column }, ERR_WARNING, pass_unit });
    }

    static std::string vformat(std::string fmt, va_list args) {
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), fmt.c_str(), args);
        return std::string(buffer);
    }
};