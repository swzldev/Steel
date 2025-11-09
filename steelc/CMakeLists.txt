# CMakeList.txt : CMake project for steelc, include source and define
# project specific logic here.
#

set(SOURCES
    "src/compiler.cpp"
    "src/config/console_args.cpp"
    "src/parser/types/container_types.cpp"
    "src/parser/types/data_type.cpp"
    "src/stproj/source_file.cpp"
    "src/interpreter/interpreter.cpp"
    "src/interpreter/interpreter_visitor.cpp"
    "src/lexer/lexer.cpp"
    "src/lexer/token_utils.cpp"
    "src/main.cpp"
    "src/parser/ast/ast_visitor.cpp"
    "src/parser/modules/module_manager.cpp"
    "src/parser/parser.cpp"
    "src/parser/parser_utils.cpp"
    "src/parser/semantics/declaration_collector.cpp"
    "src/parser/semantics/flow_analyzer.cpp"
    "src/parser/semantics/import_resolver.cpp"
    "src/parser/semantics/init_checker.cpp"
    "src/parser/semantics/name_resolver.cpp"
    "src/parser/semantics/type_checker.cpp"
    "src/parser/semantics/type_resolver.cpp"
    "src/parser/symbolics/import_table.cpp"
    "src/parser/symbolics/symbol_resolver.cpp"
    "src/parser/symbolics/symbol_table.cpp"
    "src/parser/types/core.cpp"
    "src/parser/types/custom_type.cpp"
    "src/stproj/stproj_file.cpp"
    "src/parser/types/type_utils.cpp"
    "src/utils/iclonable.h"
    "src/parser/ast/statements/control_flow/break_statement.h"
    "src/interpreter/classes/break_out.h"
    "src/parser/semantics/generic_substitutor.h"
    "src/parser/semantics/generic_substitutor.cpp"
    "src/parser/ast/declarations/enum_declaration.h"
    "src/parser/types/enum_type.h"
    "src/parser/types/enum_type.cpp" "src/parser/ast/enums/enum_option.h" "src/interpreter/debugging/interpreter_debugger.h" "src/interpreter/debugging/debug_console.h" "src/interpreter/debugging/debug_console.cpp")

# Add source to this project's executable.
add_executable ( steelc ${SOURCES} )

target_include_directories(steelc PRIVATE lib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET steelc PROPERTY CXX_STANDARD 20)
endif()
