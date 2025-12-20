# Codegen

This folder contains the various interfaces and classes used for code generation.

## Overview

- `codegen`: The main internal class reponsible for orchestrating the code generation process.
- `codegen_config`: Configuration options for code generation.
- `codegen_result`: Represents the result of a code generation process, including generated artifacts and diagnostics.
- `codegen_artifact`: Represents a single generated artifact (e.g., an object file or bitcode file).
- `icode_generator`: Interface for code generators.

## Implementing your own codegen

If you want to use steel to generate your own output format, you need to create a class that implements the `icode_generator` interface. This interface provides one method, `emit(module, config)`, which takes a `mir_module` (as well as a config) as input and returns a `codegen_result` which contains the generated artifacts (e.g., files), as well as any diagnostics and additional metadata.

> Note: You are not limited to returning just one artifact; you can return multiple artifacts if needed, for example if your code generator produces both an object file and a debug info file.

Steel provides a default code generator: `llvm_code_generator`, which generates LLVM IR code. You can use this as a reference for implementing your own code generator.