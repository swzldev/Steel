# STEELC CHANGELOG

## [0.2.0-alpha]
- Added a project_builder class to unify project building logic rather than handling it all in main.
- Added a dedicated output class for handling all console output rather than using cout, cerr, etc directly.
- Added a log_file class for logging output to a file.
- Added a string_utils class for useful string manipulation functions (currently its main purpose is to provide a format function for std::string).
- Disabled toml exception usage. All toml errors should now be handled and reported using steel's error reporting systems (mostly through bad_stproj_exception).
- Codegen is now managed by the compiler class (not sure why it wasnt before).

## [0.1.2-alpha] - 26-11-2025
- Added a function to create a compile_config object directly from command line args to prevent it from clogging main.
- Removed all the cin.get() calls in main as they are not needed anymore.
- Refactor main error messages and moved some stuff around.

## [0.1.1-alpha] - 25-11-2025
- Added a code_outputter class for outputting generated files, such as llvm IR.
- Refactored console color system.
- Removed Windows.h dependency in main.cpp.
- Simplified main file.

## [0.1.0-alpha] - 25-11-2025
- Start of changelog.