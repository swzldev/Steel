# STEELC CHANGELOG

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