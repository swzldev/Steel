# Commands

This document provides an overview of the commands available in the Steel CLI toolchain.

## Available Commands

### `steelc --help`
Displays help information about the steelc command line tool.

### `steelc --version`
Displays the current version of the steelc compilation tool.

### `steelc build <stproj-file | project-folder>`
Builds the project located at the specified path. You can provide either a `.stproj` file or a project folder containing a `.stproj` file.
#### Build Command Options
- `--no-link`: Skips the linking step after building, leaving only the generated IR files.
- `--all`: Compiles all source files, ignoring the build cache.
- `--out <output-dir>`: Overrides the default output (build) directory (usually `/build/`).
- `--int <intermediate-dir>`: Overrides the default intermediate directory (usually `/build/cache/`).

### `steelc project new <project-name>`
Creates a new project structure in the current directory with the specified name.

## Global flags

These flags can be used with any command:
- `--verbose`: Enables verbose output.
