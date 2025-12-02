# STEELC CHANGELOG#

## [Unreleased]
- Added some useful build command flags
    - `--no-link`: Skips the linking step after building, leaving only the generated IR files.
    - `--verbose`: Sets the verbosity level of output to high, showing more detailed information during the build process.
- Building no longer defaults to exe and is now platform dependent:
    - Windows: .exe
    - Linux: .out
    - MacOS: .out
- The build command now accepts a project folder as well as a project file, the project folder must contain a .stproj file.

## [0.3.0-alpha] - 02-12-2025
- Added codegen support for while loops, break statements, address-of expressions, and deref expressions.
- Fixed a bug where array initializers always had mismatched types due to pointer comparison instead of type comparison.
- Added new commands that utilise a new and improved command system:
    - `steelc --help`: Displays help information about the steelc command line tool.
    - `steelc --version`: Displays the current version of the steelc compiler.
    - `steelc build <stproj file> [args]`: Builds the project at the path (similar to before yet now using the explicit command build).
    - `steelc project <option> [args]`: Project manipulation tools, current the only option is `new <name>` which creates a new project structure in the current directory with the given name.

## [0.2.0-alpha] - 30-11-2025
- Added error support to loading stproj files through the use of the new bad_stproj_exception class.

## [0.1.2-alpha] - 26-11-2025
- Added a post build command that links all the generated IR into a single executable (currently windows only, requires clang installed and on system PATH).
- Fixed a bug where no newline was appended to each compile message.

## [0.1.1-alpha] - 25-11-2025
- Improved error message for non-void functions that dont return a value.
- Generated IR is now outputted to files (in the generated intermediate directory) instead of the console.

## [0.1.0-alpha] - 25-11-2025
- Start of changelog.