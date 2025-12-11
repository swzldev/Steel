#pragma once

#include <string>
#include <vector>
#include <cstdarg>
#include <cstdio>
#include <memory>

#include <lexer/token.h>
#include <error/error_catalog.h>

#define ERROR_TOKEN(code, tk, ...)          report_error_token(code, tk, __VA_ARGS__)
#define ERROR(code, pos, ...)               report_error(code, pos, __VA_ARGS__)
#define WARN_TOKEN(code, tk, ...)           report_warning_token(code, tk, __VA_ARGS__)
#define WARN(code, pos, ...)                report_warning(code, pos, __VA_ARGS__)
#define ADVISE(code, pos, ...)              add_advice(code, __VA_ARGS__)

class compilation_pass {
public:
	compilation_pass(std::shared_ptr<compilation_unit> unit)
		: pass_unit(unit) {
	}

    std::vector<error> errors;
    std::vector<error> warnings;
    std::shared_ptr<compilation_unit> pass_unit;

    inline bool has_errors() const {
        return !errors.empty();
    }
    inline bool has_warnings() const {
        return !warnings.empty();
	}

    inline std::vector<error> get_errors() const {
        return errors;
    }
    inline std::vector<error> get_warnings() const {
        return warnings;
	}

protected:
    void report_error_token(error_code code_enum, token tk, ...);
    void report_error(error_code code_enum, position pos, ...);

    void report_warning_token(warning_code code_enum, token tk, ...);
    void report_warning(warning_code code_enum, position pos, ...);

    void add_advice(advice_code code_enum, ...);

private:
    error* last_error = nullptr;

    void add_error(error_code code_enum, size_t line, size_t column, va_list args);

    void add_warning(warning_code code_enum, size_t line, size_t column, va_list args);

    static std::string vformat(std::string fmt, va_list args);
};