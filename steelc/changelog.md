# STEELC CHANGELOG

## [Unreleased]

## [0.1.2-alpha] - 26-11-2025
- Added a function to create a compile config object directly from command line args to prevent it from clogging main.
- Added a build command that links all the generated IR into a single executable (currently windows only, requires clang installed and on path).
- Removed all the cin.get() calls in main as they are not needed anymore.
- Refactor main error messages and moved some stuff around.
- Fixed a bug where no newline was appended to each compile message.

## [0.1.1-alpha] - 25-11-2025
- Added a code outputter class for outputting generated files, such as llvm IR.
- Improved error message for non-void functions that dont return a value.
- Generated IR is now outputted to files (in the generated intermediate directory) instead of the console.
- Refactored console color system.
- Removed Windows.h dependency in main.cpp.
- Simplified main file.

## [0.1.0-alpha] - 25-11-2025
- Start of changelog.