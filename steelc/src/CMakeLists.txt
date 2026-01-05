# steelc/src/CMakeLists.txt

add_library(steelc_core)

# just to keep things tidy, we separate the source files into categories
# additionally, we store headers just for convenience.

target_sources(steelc_core PRIVATE
    # ast/
    "ast/ast.h"
    "ast/ast_fwd.h"
    "ast/ast_node.h"
    "ast/ast_visitor.cpp"
    "ast/ast_visitor.h"
    "ast/compilation_unit.h"
    "ast/declarations/conversion_declaration.h"
    "ast/declarations/declaration.h"
    "ast/declarations/enum_declaration.h"
    "ast/declarations/function_declaration.h"
    "ast/declarations/module_declaration.h"
    "ast/declarations/operator_declaration.h"
    "ast/declarations/type_declaration.h"
    "ast/declarations/variable_declaration.h"
    "ast/enums/enum_option.h"
    "ast/expressions/address_of_expression.h"
    "ast/expressions/assignment_expression.h"
    "ast/expressions/binary_expression.h"
    "ast/expressions/cast_expression.h"
    "ast/expressions/deref_expression.h"
    "ast/expressions/expression.h"
    "ast/expressions/expression_statement.h"
    "ast/expressions/function_call.h"
    "ast/expressions/identifier_expression.h"
    "ast/expressions/index_expression.h"
    "ast/expressions/initializer_list.h"
    "ast/expressions/literal.h"
    "ast/expressions/member_expression.h"
    "ast/expressions/this_expression.h"
    "ast/expressions/unary_expression.h"
    "ast/generics/generic_parameter.h"
    "ast/statements/code_block.h"
    "ast/statements/import_statement.h"
    "ast/statements/inline_if.h"
    "ast/statements/control_flow/break_statement.h"
    "ast/statements/control_flow/for_loop.h"
    "ast/statements/control_flow/if_statement.h"
    "ast/statements/control_flow/return_statement.h"
    "ast/statements/control_flow/while_loop.h"

    # ast_lowering/
    "ast_lowering/mir_lowerer.cpp"
    "ast_lowering/mir_lowerer.h"
    "ast_lowering/mir_lowering_visitor.cpp"
    "ast_lowering/mir_lowering_visitor.h"

    # ast_passes/
    "ast_passes/compile_time_computator.cpp"
    "ast_passes/compile_time_computator.h"
    "ast_passes/declaration_collector.cpp"
    "ast_passes/declaration_collector.h"
    "ast_passes/flow_analyzer.cpp"
    "ast_passes/flow_analyzer.h"
    "ast_passes/generic_substitutor.cpp"
    "ast_passes/generic_substitutor.h"
    "ast_passes/import_resolver.cpp"
    "ast_passes/import_resolver.h"
    "ast_passes/init_checker.cpp"
    "ast_passes/init_checker.h"
    "ast_passes/name_resolver.cpp"
    "ast_passes/name_resolver.h"
    "ast_passes/type_checker.cpp"
    "ast_passes/type_checker.h"
    "ast_passes/type_resolver.cpp"
    "ast_passes/type_resolver.h"
    "ast_passes/classes/computed_value.cpp"
    "ast_passes/classes/computed_value.h"
    "ast_passes/helpers/function_filter.cpp"
    "ast_passes/helpers/function_filter.h"
    "ast_passes/util/entity_refresher.cpp"
    "ast_passes/util/entity_refresher.h"

    # building/
    "building/build_config.h"
    "building/code_outputter.cpp"
    "building/code_outputter.h"
    "building/project_builder.cpp"
    "building/project_builder.h"
    "building/cache/artifact_metadata.h"
    "building/cache/build_cache_file.cpp"
    "building/cache/build_cache_file.h"
    "building/cache/file_hasher.cpp"
    "building/cache/file_hasher.h"
    "building/cache/source_metadata.h"
    "building/cache/vars_file.cpp"
    "building/cache/vars_file.h"

    # codegen/
    "codegen/codegen.cpp"
    "codegen/codegen.h"
    "codegen/codegen_config.h"
    "codegen/codegen_info.h"
    "codegen/codegen_result.h"
    "codegen/code_artifact.h"
    "codegen/icode_generator.h"
    "codegen/cleanup/cleanup_action.cpp"
    "codegen/cleanup/cleanup_action.h"
    "codegen/cleanup/scope.h"
    "codegen/constants/codegen_constants.h"
    "codegen/error/codegen_exception.h"
    "codegen/memory/lvalue.cpp"
    "codegen/memory/lvalue.h"
    "codegen/memory/variable.h"
    "codegen/naming/name_mangler.cpp"
    "codegen/naming/name_mangler.h"
    "codegen/sys/system_formats.cpp"
    "codegen/sys/system_formats.h"

    # codegen_llvm/
    "codegen_llvm/llvm_code_generator.cpp"
    "codegen_llvm/llvm_code_generator.h"
    "codegen_llvm/builders/llvm_expression_builder.cpp"
    "codegen_llvm/builders/llvm_expression_builder.h"
    "codegen_llvm/builders/llvm_function_builder.cpp"
    "codegen_llvm/builders/llvm_function_builder.h"
    "codegen_llvm/type_handling/llvm_type_converter.cpp"
    "codegen_llvm/type_handling/llvm_type_converter.h"
    "codegen_llvm/type_handling/llvm_type_utils.cpp"
    "codegen_llvm/type_handling/llvm_type_utils.h"
    "codegen_llvm/writers/llvm_native_writer.cpp"
    "codegen_llvm/writers/llvm_native_writer.h"
    "codegen_llvm/writers/llvm_writer.cpp"
    "codegen_llvm/writers/llvm_writer.h"

    # compiler/
    "compiler/compilation_pass.cpp"
    "compiler/compilation_pass.h"
    "compiler/compiler.cpp"
    "compiler/compiler.h"
    "compiler/compile_config.h"

    # error/
    "error/advice.h"
    "error/error.h"
    "error/error_catalog.h"
    "error/error_printer.cpp"
    "error/error_printer.h"

    # input/
    "input/console_args.cpp"
    "input/console_args.h"
    "input/commands/steelc_command.cpp"
    "input/commands/steelc_command.h"
    "input/commands/steelc_commands_impl.cpp"
    "input/commands/steelc_commands_impl.h"
    "input/commands/steelc_command_id.h"
    "input/commands/steelc_command_manager.cpp"
    "input/commands/steelc_command_manager.h"
    "input/commands/flags/command_flags.h"
    "input/commands/flags/flag_config.cpp"
    "input/commands/flags/flag_config.h"

    # lexer/
    "lexer/lexer.cpp"
    "lexer/lexer.h"
    "lexer/token.h"
    "lexer/token_type.h"
    "lexer/token_utils.cpp"
    "lexer/token_utils.h"

    # linking/
    "linking/icode_linker.h"
    "linking/linker.cpp"
    "linking/linker.h"
    "linking/link_config.h"
    "linking/link_data.h"
    
    "linking/link_result.h"
    "linking/impl/coff_linker.cpp"
    "linking/impl/coff_linker.h"
    "linking/sys/system_linker.cpp"
    "linking/sys/system_linker.h"

    # mir/
    "mir/mir_block.h"
    "mir/mir_function.h"
    "mir/mir_fwd.h"
    "mir/mir_instr.h"
    "mir/mir_module.h"
    "mir/mir_operand.h"
    "mir/mir_type.h"
    "mir/mir_value.h"
    "mir/builders/mir_builder.cpp"
    "mir/builders/mir_builder.h"
    "mir/pretty/mir_printer.cpp"
    "mir/pretty/mir_printer.h"

    # modules/
    "modules/module_info.h"
    "modules/module_manager.cpp"
    "modules/module_manager.h"

    # output/
    "output/log_file.cpp"
    "output/log_file.h"
    "output/output.cpp"
    "output/output.h"

    # parser/
    "parser/parser.cpp"
    "parser/parser.h"
    "parser/parser_utils.cpp"
    "parser/parser_utils.h"

    # representations/
    "representations/entities/entities_fwd.h"
    "representations/entities/entity.cpp"
    "representations/entities/entity.h"
    "representations/entities/entity_ref.cpp"
    "representations/entities/entity_ref.h"
    "representations/entities/enum_entity.cpp"
    "representations/entities/enum_entity.h"
    "representations/entities/function_entity.cpp"
    "representations/entities/function_entity.h"
    "representations/entities/generic_param_entity.cpp"
    "representations/entities/generic_param_entity.h"
    "representations/entities/module_entity.cpp"
    "representations/entities/module_entity.h"
    "representations/entities/type_entity.cpp"
    "representations/entities/type_entity.h"
    "representations/entities/variable_entity.cpp"
    "representations/entities/variable_entity.h"
    "representations/types/container_types.cpp"
    "representations/types/container_types.h"
    "representations/types/core.cpp"
    "representations/types/core.h"
    "representations/types/custom_type.cpp"
    "representations/types/custom_type.h"
    "representations/types/data_type.cpp"
    "representations/types/data_type.h"
    "representations/types/enum_type.cpp"
    "representations/types/enum_type.h"
    "representations/types/function_type.cpp"
    "representations/types/function_type.h"
    "representations/types/types_fwd.h"
    "representations/types/type_utils.cpp"
    "representations/types/type_utils.h"

    # steelc/
    "steelc/language_constants.h"
    "steelc/steelc_definitions.h"

    # stproj/
    "stproj/bad_stproj_exception.h"
    "stproj/source_file.cpp"
    "stproj/source_file.h"
    "stproj/stproj_file.cpp"
    "stproj/stproj_file.h"
    "stproj/stproj_generator.cpp"
    "stproj/stproj_generator.h"

    # symbolics/
    "symbolics/import_table.cpp"
    "symbolics/import_table.h"
    "symbolics/lookup_result.cpp"
    "symbolics/lookup_result.h"
    "symbolics/overlay_table.cpp"
    "symbolics/overlay_table.h"
    "symbolics/symbol_error.h"
    "symbolics/symbol_resolver.cpp"
    "symbolics/symbol_resolver.h"
    "symbolics/symbol_table.cpp"
    "symbolics/symbol_table.h"
    "symbolics/type_symbols.cpp"
    "symbolics/type_symbols.h"

    # sys/
    "sys/host_defs.h"
    "sys/platform.cpp"
    "sys/platform.h"
    "sys/shell.cpp"
    "sys/shell.h"
    "sys/target_triple.cpp"
    "sys/target_triple.h"

    # utils/
    "utils/console_colors.cpp"
    "utils/console_colors.h"
    "utils/iclonable.h"
    "utils/maybe.h"
    "utils/path_utils.cpp"
    "utils/path_utils.h"
    "utils/string_utils.h"
    "utils/string_utils.cpp"
)

target_include_directories(steelc_core
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"

PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
)

target_link_libraries(steelc_core PUBLIC
    ${LLVM_LIBS}
)
