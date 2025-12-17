# steelc

# Sources are organized in alphabetical order within their respective folders

set(SOURCES
    "src/main.cpp"
    # ast/
    "src/ast/ast.h"
    "src/ast/ast_fwd.h"
    "src/ast/ast_node.h"
    "src/ast/ast_visitor.cpp"
    "src/ast/ast_visitor.h"
    "src/ast/compilation_unit.h"
    "src/ast/declarations/conversion_declaration.h"
    "src/ast/declarations/declaration.h"
    "src/ast/declarations/enum_declaration.h"
    "src/ast/declarations/function_declaration.h"
    "src/ast/declarations/module_declaration.h"
    "src/ast/declarations/operator_declaration.h"
    "src/ast/declarations/type_declaration.h"
    "src/ast/declarations/variable_declaration.h"
    "src/ast/enums/enum_option.h"
    "src/ast/expressions/address_of_expression.h"
    "src/ast/expressions/assignment_expression.h"
    "src/ast/expressions/binary_expression.h"
    "src/ast/expressions/cast_expression.h"
    "src/ast/expressions/deref_expression.h"
    "src/ast/expressions/expression.h"
    "src/ast/expressions/expression_statement.h"
    "src/ast/expressions/function_call.h"
    "src/ast/expressions/identifier_expression.h"
    "src/ast/expressions/index_expression.h"
    "src/ast/expressions/initializer_list.h"
    "src/ast/expressions/literal.h"
    "src/ast/expressions/member_expression.h"
    "src/ast/expressions/this_expression.h"
    "src/ast/expressions/unary_expression.h"
    "src/ast/generics/generic_parameter.h"
    "src/ast/statements/code_block.h"
    "src/ast/statements/import_statement.h"
    "src/ast/statements/inline_if.h"
    "src/ast/statements/control_flow/break_statement.h"
    "src/ast/statements/control_flow/for_loop.h"
    "src/ast/statements/control_flow/if_statement.h"
    "src/ast/statements/control_flow/return_statement.h"
    "src/ast/statements/control_flow/while_loop.h"

    # ast_lowering/
    "src/ast_lowering/mir_lowerer.cpp"
    "src/ast_lowering/mir_lowerer.h"
    "src/ast_lowering/mir_lowering_visitor.cpp"
    "src/ast_lowering/mir_lowering_visitor.h"

    # ast_passes/
    "src/ast_passes/compile_time_computator.cpp"
    "src/ast_passes/compile_time_computator.h"
    "src/ast_passes/declaration_collector.cpp"
    "src/ast_passes/declaration_collector.h"
    "src/ast_passes/flow_analyzer.cpp"
    "src/ast_passes/flow_analyzer.h"
    "src/ast_passes/generic_substitutor.cpp"
    "src/ast_passes/generic_substitutor.h"
    "src/ast_passes/import_resolver.cpp"
    "src/ast_passes/import_resolver.h"
    "src/ast_passes/init_checker.cpp"
    "src/ast_passes/init_checker.h"
    "src/ast_passes/name_resolver.cpp"
    "src/ast_passes/name_resolver.h"
    "src/ast_passes/type_checker.cpp"
    "src/ast_passes/type_checker.h"
    "src/ast_passes/type_resolver.cpp"
    "src/ast_passes/type_resolver.h"
    "src/ast_passes/classes/computed_value.cpp"
    "src/ast_passes/classes/computed_value.h"
    "src/ast_passes/helpers/function_filter.cpp"
    "src/ast_passes/helpers/function_filter.h"
    "src/ast_passes/util/entity_refresher.cpp"
    "src/ast_passes/util/entity_refresher.h"

    # building/
    "src/building/build_config.h"
    "src/building/code_outputter.cpp"
    "src/building/code_outputter.h"
    "src/building/project_builder.cpp"
    "src/building/project_builder.h"
    "src/building/cache/build_cache_file.cpp"
    "src/building/cache/build_cache_file.h"
    "src/building/cache/file_hasher.cpp"
    "src/building/cache/file_hasher.h"
    "src/building/cache/file_metadata.h"
    "src/building/ir/ir_generator.cpp"
    "src/building/ir/ir_generator.h"
    "src/building/linking/link_error.h"
    "src/building/linking/project_linker.cpp"
    "src/building/linking/project_linker.h"

    # codegen/
    "src/codegen/codegen.cpp"
    "src/codegen/codegen.h"
    "src/codegen/codegen_result.h"
    "src/codegen/code_artifact.h"
    "src/codegen/icode_generator.h"
    "src/codegen/cleanup/cleanup_action.cpp"
    "src/codegen/cleanup/cleanup_action.h"
    "src/codegen/cleanup/scope.h"
    "src/codegen/constants/codegen_constants.h"
    "src/codegen/error/codegen_exception.h"
    "src/codegen/modules/module_holder.h"
    "src/codegen/memory/lvalue.cpp"
    "src/codegen/memory/lvalue.h"
    "src/codegen/memory/variable.h"
    "src/codegen/naming/name_mangler.cpp"
    "src/codegen/naming/name_mangler.h"

    # codegen_llvm/
    "src/codegen_llvm/codegen_env.cpp"
    "src/codegen_llvm/codegen_env.h"
    "src/codegen_llvm/llvm_code_generator.cpp"
    "src/codegen_llvm/llvm_code_generator.h"
    "src/codegen_llvm/builders/llvm_expression_builder.cpp"
    "src/codegen_llvm/builders/llvm_expression_builder.h"
    "src/codegen_llvm/builders/llvm_function_builder.cpp"
    "src/codegen_llvm/builders/llvm_function_builder.h"
    "src/codegen_llvm/type_handling/llvm_type_converter.cpp"
    "src/codegen_llvm/type_handling/llvm_type_converter.h"
    "src/codegen_llvm/type_handling/llvm_type_utils.cpp"
    "src/codegen_llvm/type_handling/llvm_type_utils.h"

    # compiler/
    "src/compiler/compilation_pass.cpp"
    "src/compiler/compilation_pass.h"
    "src/compiler/compiler.cpp"
    "src/compiler/compiler.h"

    # config/
    "src/config/bad_arg_exception.h"
    "src/config/compile_config.cpp"
    "src/config/compile_config.h"

    # error/
    "src/error/advice.h"
    "src/error/error.h"
    "src/error/error_catalog.h"
    "src/error/error_printer.cpp"
    "src/error/error_printer.h"

    # input/
    "src/input/console_args.cpp"
    "src/input/console_args.h"
    "src/input/commands/steelc_command.cpp"
    "src/input/commands/steelc_command.h"
    "src/input/commands/steelc_commands_impl.cpp"
    "src/input/commands/steelc_commands_impl.h"
    "src/input/commands/steelc_command_id.h"
    "src/input/commands/steelc_command_manager.cpp"
    "src/input/commands/steelc_command_manager.h"
    "src/input/commands/flags/command_flags.h"
    "src/input/commands/flags/flag_config.cpp"
    "src/input/commands/flags/flag_config.h"

    # lexer/
    "src/lexer/lexer.cpp"
    "src/lexer/lexer.h"
    "src/lexer/token.h"
    "src/lexer/token_type.h"
    "src/lexer/token_utils.cpp"
    "src/lexer/token_utils.h"

    # mir/
    "src/mir/mir_block.h"
    "src/mir/mir_function.h"
    "src/mir/mir_fwd.h"
    "src/mir/mir_instr.h"
    "src/mir/mir_module.h"
    "src/mir/mir_operand.h"
    "src/mir/mir_type.h"
    "src/mir/mir_value.h"
    "src/mir/builders/mir_builder.cpp"
    "src/mir/builders/mir_builder.h"
    "src/mir/pretty/mir_printer.cpp"
    "src/mir/pretty/mir_printer.h"

    # modules/
    "src/modules/module_info.h"
    "src/modules/module_manager.cpp"
    "src/modules/module_manager.h"

    # output/
    "src/output/log_file.cpp"
    "src/output/log_file.h"
    "src/output/output.cpp"
    "src/output/output.h"

    # parser/
    "src/parser/parser.cpp"
    "src/parser/parser.h"
    "src/parser/parser_utils.cpp"
    "src/parser/parser_utils.h"

    # representations/
    "src/representations/entities/entities_fwd.h"
    "src/representations/entities/entity.cpp"
    "src/representations/entities/entity.h"
    "src/representations/entities/entity_ref.cpp"
    "src/representations/entities/entity_ref.h"
    "src/representations/entities/enum_entity.cpp"
    "src/representations/entities/enum_entity.h"
    "src/representations/entities/function_entity.cpp"
    "src/representations/entities/function_entity.h"
    "src/representations/entities/generic_param_entity.cpp"
    "src/representations/entities/generic_param_entity.h"
    "src/representations/entities/module_entity.cpp"
    "src/representations/entities/module_entity.h"
    "src/representations/entities/type_entity.cpp"
    "src/representations/entities/type_entity.h"
    "src/representations/entities/variable_entity.cpp"
    "src/representations/entities/variable_entity.h"
    "src/representations/types/container_types.cpp"
    "src/representations/types/container_types.h"
    "src/representations/types/core.cpp"
    "src/representations/types/core.h"
    "src/representations/types/custom_type.cpp"
    "src/representations/types/custom_type.h"
    "src/representations/types/data_type.cpp"
    "src/representations/types/data_type.h"
    "src/representations/types/enum_type.cpp"
    "src/representations/types/enum_type.h"
    "src/representations/types/function_type.cpp"
    "src/representations/types/function_type.h"
    "src/representations/types/types_fwd.h"
    "src/representations/types/type_utils.cpp"
    "src/representations/types/type_utils.h"

    # steelc/
    "src/steelc/language_constants.h"
    "src/steelc/steelc_definitions.h"

    # stproj/
    "src/stproj/bad_stproj_exception.h"
    "src/stproj/source_file.cpp"
    "src/stproj/source_file.h"
    "src/stproj/stproj_file.cpp"
    "src/stproj/stproj_file.h"
    "src/stproj/stproj_generator.cpp"
    "src/stproj/stproj_generator.h"

    # symbolics/
    "src/symbolics/import_table.cpp"
    "src/symbolics/import_table.h"
    "src/symbolics/lookup_result.cpp"
    "src/symbolics/lookup_result.h"
    "src/symbolics/overlay_table.cpp"
    "src/symbolics/overlay_table.h"
    "src/symbolics/symbol_error.h"
    "src/symbolics/symbol_resolver.cpp"
    "src/symbolics/symbol_resolver.h"
    "src/symbolics/symbol_table.cpp"
    "src/symbolics/symbol_table.h"
    "src/symbolics/type_symbols.cpp"
    "src/symbolics/type_symbols.h"

    # utils/
    "src/utils/console_colors.cpp"
    "src/utils/console_colors.h"
    "src/utils/iclonable.h"
    "src/utils/maybe.h"
    "src/utils/path_utils.cpp"
    "src/utils/path_utils.h"
    "src/utils/string_utils.h"
    "src/utils/string_utils.cpp"

    # tests/
    "tests/test.cpp"
    "tests/test.h"

    # tests/lexer_tests/
    "tests/lexer_tests/lexer_tests.cpp"
    "tests/lexer_tests/lexer_tests.h"
)

# Add source to this project's executable.
add_executable ( steelc ${SOURCES} )

# LLVM
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(LLVM_LIBS
    core
    support
    irreader
    linker
    bitwriter
)
target_link_libraries(steelc PRIVATE ${LLVM_LIBS})

# Support for includes in /lib/, /src/ and /tests/
target_include_directories(steelc PRIVATE "lib")
target_include_directories(steelc PRIVATE "src")
target_include_directories(steelc PRIVATE "tests")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET steelc PROPERTY CXX_STANDARD 20)
endif()
