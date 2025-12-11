#include "compilation_pass.h"

void compilation_pass::report_error_token(error_code code_enum, token tk, ...) {
    va_list args;
    va_start(args, tk);
    add_error(code_enum, tk.pos.line, tk.pos.column, args);
    va_end(args);
}
void compilation_pass::report_error(error_code code_enum, position pos, ...) {
    va_list args;
    va_start(args, pos);
    add_error(code_enum, pos.line, pos.column, args);
    va_end(args);
}

void compilation_pass::report_warning_token(warning_code code_enum, token tk, ...) {
    va_list args;
    va_start(args, tk);
    add_warning(code_enum, tk.pos.line, tk.pos.column, args);
    va_end(args);
}
void compilation_pass::report_warning(warning_code code_enum, position pos, ...) {
    va_list args;
    va_start(args, pos);
    add_warning(code_enum, pos.line, pos.column, args);
    va_end(args);
}

void compilation_pass::add_advice(advice_code code_enum, ...) {
    if (last_error == nullptr) {
        return;
    }

    advice_info info = error_catalog::get_advice_info(code_enum);
	va_list args;
    va_start(args, code_enum);
    info.message = vformat(info.message, args);
    va_end(args);

	last_error->advices.push_back({ last_error, info, });
}

void compilation_pass::add_error(error_code code_enum, size_t line, size_t column, va_list args) {
    error_info info = error_catalog::get_error_info(code_enum);
    info.message = vformat(info.message, args);
    errors.push_back({ info, { line, column }, ERR_ERROR, pass_unit });
	last_error = &errors.back();
}

void compilation_pass::add_warning(warning_code code_enum, size_t line, size_t column, va_list args) {
    error_info info = error_catalog::get_warning_info(code_enum);
    info.message = vformat(info.message, args);
    warnings.push_back({ info, { line, column }, ERR_WARNING, pass_unit });
}

std::string compilation_pass::vformat(std::string fmt, va_list args) {
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt.c_str(), args);
    return std::string(buffer);
}
