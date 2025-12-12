# STEELC CHANGELOG

## [Unreleased]
- Added support for bitcode output in addition to assembly output (bitcode will now be default).
- Changed some of the default output directories:
    - Build output directory: `/build/`
    - Intermediate output directory: `/build/intermediate/`
    - IR output directory: `/build/intermediate/IR/`
- Added a new build flag `--llvm-asm` which outputs the generated LLVM IR as assembly files (.ll) instead of bitcode files (.bc). Useful for debugging and inspecting the generated IR.
- Fixed a bug where constructors dont have identifiers therefore the declaration had an invalid position.

## [0.5.0] - 10-12-2025
- Added support for the '::' operator for static scope resolution.
- Added support for module member resolution without having to import.
- The build cache now only saves after full successful builds which prevents it from caching if output or post build steps fail.
- Added a check if codegen fails instead of just only returning the successfully generated ir.
- Added much better error support for ambiguous name resolution.
- Updated build cache to version 2, which now tracks the version of steelc used to build the cached files, preventing issues when updating.
- Fixed a bug where the message of codegen exceptions was always empty.
- Fixed a bug where you could lookup any submodule without having to reference the parent module first, e.g. `foo::bar::baz` and `bar::baz` would both work.
- Huge behind the scenes symbol and name resolution changes.

## [0.4.0] - 04-12-2025
- Added some useful build command flags:
    - `--no-link`: Skips the linking step after building, leaving only the generated IR files.
    - `--all`: Compiles all source files ignoring the build cache.
    - `--out <output-dir>`: Overrides the default output (build) directory (usually `/build/`).
    - `--int <intermediate-dir>`: Overrides the default intermediate directory (usually `/build/cache/`).
- Added a global flag system for certain flags that can be used with any command:
    - `--verbose`: Enables verbose output.
- Building no longer defaults to exe and is now platform dependent:
    - Windows: .exe
    - MacOS: .out
    - Linux: .out
- The build command now accepts a project folder as well as a project file, the project folder must contain a .stproj file.
- Updated the vs code extension to work with the new steelc commands.
- Removed the 'run' command from the vs code extension, may reimplement it in the future for now its pretty redundant with build + manual execution.
- Added a build cache system that stores build artifacts to speed up subsequent builds.
- Compilation is now fully skipped if no changes are detected in the source files since the last build (and --all is not used).
- Fixed a bug where the path of the project file wasnt being checked before parsing leading to a crash.
- Clang build is now a part of the building process and no longer a post build command (which was janky).

## [0.3.0] - 02-12-2025
- Added codegen support for while loops, break statements, address-of expressions, and deref expressions.
- Fixed a bug where array initializers always had mismatched types due to pointer comparison instead of type comparison.
- Added new commands that utilise a new and improved command system:
    - `steelc --help`: Displays help information about the steelc command line tool.
    - `steelc --version`: Displays the current version of the steelc compiler.
    - `steelc build <stproj file> [args]`: Builds the project at the path (similar to before yet now using the explicit command build).
    - `steelc project <option> [args]`: Project manipulation tools, current the only option is `new <name>` which creates a new project structure in the current directory with the given name.

## [0.2.0] - 30-11-2025
- Added error support to loading stproj files through the use of the new bad_stproj_exception class.

## [0.1.2] - 26-11-2025
- Added a post build command that links all the generated IR into a single executable (currently windows only, requires clang installed and on system PATH).
- Fixed a bug where no newline was appended to each compile message.

## [0.1.1] - 25-11-2025
- Improved error message for non-void functions that dont return a value.
- Generated IR is now outputted to files (in the generated intermediate directory) instead of the console.

## [0.1.0] - 25-11-2025
- Start of changelog.