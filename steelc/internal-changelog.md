# STEELC CHANGELOG

## [Unreleased]
- Added a verbosity option to output as well as a verbose() method which only outputs if the verbosity level is >= VERBOSITY_HIGH. Debug builds default to VERBOSITY_HIGH.
- Small rewrite of the command system to allow for easier addition of new commands and better flag error handling.
- The code output system is now apart of the build system rather than just general "output", this means it can now rely on the build config rather than a seperate output config that reuses a lot of the same data.

## [0.3.0] - 02-12-2025
- Added a tests folder for language tests.
- Added loop support into the codegen_env class (for break, continue, etc).
- Added an is_constant method to expression ast nodes that indicates whether the expression can be evaluated at compile time. e.g. 5 + 3 -> 8.
- Added a compile_time_computator and visitor for evaluating constant expressions at compile time.
- Array types now contain a `size_t size` member which will need to be calculated at compile-time, if its not a constant value, an error will be raised. Currently this is unused and arrays are still broken until the compile-time computator is fully implemented.
- Implemented a new command system that includes a command manager, command class, and command implementation file for defining command behaviour.
- Created a steelc_definitions file for defining steelc specific constants (such as version number).

## [0.2.0-alpha] - 30-11-2025
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