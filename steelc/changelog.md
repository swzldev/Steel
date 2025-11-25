# STEELC CHANGELOG


## 0.1.0-alpha.1

### 18-09-2025
- Pausing work on generics
- Begin work on interface functionality
	- Re-allowed parsing function declarations without bodies,
	nescessary for allowing interfaces to work.
	- Added support for derived classes using ':'.
	- Type checker now checks for method implementations in base classes.
	- Declaration collector checks if interfaces or structs are derived.
- The type resolver now attempts to resolve base types of classes.
- Overrides are now supported by the parser and type checker, using
  the 'override' keyword.
- Implemented the 'constructor' keyword and begun work on merging
  constructors into the function_declaration node instead of being
  seperate.
- Discovered a bug whereby type inference occurs in the type checker
  but type info is required in the name resolver. Need to fix this.
- Interfaces fully working @ 23:29.

- TODO:
	- Fix inference bug
	- Fix class member vars being broken

### 19-09-2025
- Added support for base class lookups for methods & class members.
  Class member variables are still broken anyway for now so unable to test
  if it works.
- Moved method lookup to the type checker so that it can use
  type inference to resolve types.
- Updated the flow analyzer:
	- If the return value type is unknown we dont report any mismatch errors
	  to make the errors less confusing, unknown types will be reported
	  elsewhere anyway (i think).
	- Current returns is set to true immediately at any return reguardless of
	  the type checking that happens after to reduce confusion. Before, if a
	  return statement had a type mismatch it would give a "function does not
	  return" error which didnt really make sense. The type checking should
	  really be done in the type checker and i may move it in the future.
- Moved constructor collection from get_function_candidates into a new function
  get_ctor_candidates, to make it easier to check for constructor-specific errors.
- Updated the name resolver to use the new get_ctor_candidates function.
- Added a member to the function_call node to store the type (if its a constructor
  call). This can be used to check if a call is a constructor without needing
  to lookup every time or check the candidates.
- Added a check into the type checker when attempting to resolve an unknown type
  member, to reduce confusion. An error should be thrown elsewhere.
- Moved the type checking of return statements into the type checker and out
  of the flow analyzer. The flow analyzer should only be concerned with
  control flow, not type checking.
- Completed return statement testing, 100% working in both constructors and
  normal functions, emits errors correctly.
- Fixed a bug in the interpreter visitor where set_var wasnt checking the this
  pointer first, switched it to just rely on get_var and update it using the shared
  pointer returned.
- Fixed class fields being broken, they are now fully functional, but i may change
  the way they are declared later.
- Functions no longer need to specify the return type, if it isnt matched it will
  default as void by the parser.
- Fixed a bug where a reference was used when looking up a type tree, causing
  types to be set to nullptr.
- Fixed a bug where method arguments were being evaluated after setting a new
  'this' pointer, causing issues if the arguments referenced class members.

  TODO:
    - Begin working on more implicit conversion support, specifically on
	  interface pointers.

### 20-09-2025
- Added a check for circular inheritance in the type checker.
- Added a function is_valid_downcast for checking if a type can be downcasted to
  a base type such as a base class or interface.
- Added a new function for checking member access, allowing for single pointer
  dereferencing when accessing members of a class or interface. Similar to
  '->' in C++, but without the use of a seperate operator.
- Added vftable logic to the interpreter, now works for downcasted interfaces,
  it WOULD work for downcast classes too but there is currently no way to create
  a virtual function apart from in interfaces where it defaults to being virtual.
- Added a new ast node deref_expression, allowing for explicit dereferencing
  of pointers. This allows for support for 2+ level pointers that cannot be
  dereferenced using the '.' operator.
- Added type checks for deref expressions, ensuring that only pointer types
  can be dereferenced.
- Added support for deref expressions to the interpreter.
- Added a flag to enable/disable generics, so that early release builds do not
  have access to half finished generics functionality.
- Added built in functions: Read(), ReadKey(), ReadInt(), Wait(float seconds),
  SetConsolePos(i32 x, i32 y).
- Type checker now checks for unknown type assignments to prevent unnecessary
  errors.
- Fixed some bugs:
  - Fixed a bug where method calls were being passed a copy of the object as
    the 'this' pointer instead of the actual object, causing methods to be
	modifying a clone and not themselves.
  - Fixed a bug in the init checker where it would try to check the init state
    of a variable before it had been resolved, causing a nullptr dereference.
  - Fixed a bug in the type checker where it would check if a variable is const
    without checking if its even been resolved, causing a nullptr dereference.
  - Fixed a bug in the type checker where it would invalidate dereferences to non
    variables instead of just checking its a pointer type. Chained pointer
    derefences now work as intended.
  - Fixed a bug where the type checker wasnt entering while and for loop bodies.
- Name resolver now outputs an error for constructor call ambiguity.
- Added a temporary override_return_type member to function call nodes because
  builtin functions such as ReadInt() need to have a return type but they are
  unable to contain actual function declarations as it breaks how they are
  interpreted. This will be removed when the language is switched to being
  compiled.

### 21-09-2025
- Added support for array initializers and array variables in the interpreter.
- Fixed some bugs:
    - Fixed a bug in the interpreter where the 'this' object was being pushed onto
	  the this stack before being set, causing the first object on the stack to
	  be a null pointer.
- Added a new function GetKey() to check if a key is being pressed without halting
  the program like ReadKey().
- Added overrides for the 3 container types to allow support for indexing on arrays
  and pointers (although pointer indexing is not yet functional).

### 14-10-2025
- Huge type system rewrite - ended up liking the original system more so rolled back.
- Some of the changes from the new system have been backported and so unfortunately
  there may be some missing changes in the changelog but that isnt so important
  in this stage anyway.
- != is no longer a virtual operator in data_type, as it always just calls !(*this == other)
  anyway.
- Casting into data_type subclasses is now built-in to the data_type class, which makes
  it significantly easier to write and also more safe (backported).
- Added a new DT_CUSTOM data type so that custom types can be detected easily from the
  base class (data_type) and no longer just uses DT_UNKNOWN which should be reserved
  only for the UNKNOWN data type (backported).
- data_type now derives from std::enable_shared_from_this to allow as_ functions to work
  correctly (backported).
- data_type's == operator now uses a type_ptr argument which means dereferencing types
  is no longer needed during comparison (backported).
- Added a language_constants namespace to store widely used language constants, for example
  the name of the entry point (backported).
- Updated the interpreter with a lot of changes made to support the new type system (however
  these changes are better for the old system too so have been backported):
    - The interpreter now uses type_ptr instead of raw data_type objects which reduces the need
	  a bunch of dereferencing.
	- The interpreter now correctly sets up the type for pointers and arrays instead of just
      creating a new data_type object with the correct type enum which should be illegal.
	- The interpreter now uses new utility functions new_... to create new objects at runtime
	  which avoids std::make_shared spam.
	- The interpreter now throws an actual c++ exception when a runtime error occurs instead of
	  just printing an error and exiting, which is just better for overall code flow and allows
	  an additional "Execution failed." message as well as any nescessary clean-up which may
	  be needed in the future.

### 16-10-2025
- Renamed block_statement to code_block and added a flag is_body for denoting if its the body of
  another node or just a standalone code block (backported).
- Updated the parser to return an accurate ast node type instead of just a generate ast_ptr (backported).
- Added generic support for type declarations.

### 17-10-2025
- Backported some more features in the semantic passes to re-enable generic support.
- Added some utility functions to data_type:
    - is_void()
	- is_integer()
	- is_floating_point()
	- is_numeric()
	- is_character()
	- is_text()
- Added a position member to data_type for better error reporting.

### 19-10-2025
- Added a new version of make_ast that supports a direct position object rather than a token.
- The parser now sets the is_body flag of standalone and non-standalone code blocks correctly.
- Added support for break statements within loops.

### 20-10-2025
- Renamed i16, i32, and i64 to short, int and long.

### 21-10-2025
- Redesigned the generic system:
    - Generic types now store an index instead of a direct shared pointer to the parameter.
	- This allows lookup to be re-done within the type checker which means it is much easier
	  and safer when type checking generic function instantiations.
	- Generic types also no longer subsitute, as the functions are now completely seperate, and not
	  just substituted whilst type checking. Instead, the generic types are completely replaced
	  with the concrete types, which also means that type checking is much simpler.

### 23-10-2025
- All appropriate AST nodes now have their clone function defined.
- Added a generic_args member to data_type for use with generic types.

### 25-10-2025
- Fixed a bug in the type resolver where pointer/array types do not have their base type resolved.

### 28-10-2025
- Moved the get_ctor_candidates function into type_utils as it no longer looks up the type by name,
  instead, it takes the type as an argument.
- Constructor candidates are now resolved in the type checker, allowing for generic type constructors.
- Added a new generic_substitutor pass for immediate substitution instead of runtime looking during type
  checking which is bloated and unnescessary.
- Added a generic function and type map to the type checker so that generic instantiations can be stored
  and reused.
- When instantiating types the new type is set as the constructors return type, as otherwise it points to
  the non-instantiated version.
- Custom types now store instantiated versions as well in a generic_type_map so that custom_type::get()
  can return the correct instantiated type rather than the generic one.
- Begin working on enum support:
    - Added 'enum' as a keyword (TT_ENUM).
	- Created a new ast node enum_declaration for storing enum declarations.
	- Created a new data type enum_type for representing different enums which allows them to be
	  clearly seperated from other types and ensures type safety.
	- The parser can now parse enum declarations.

### 30-10-2025
- Continued working on enum support:
    - enum_type is now fully complete.
	- The type resolver now uses a simplified function resolve_custom(type) for resolving custom types
	  including enums, which allows for early return without bypassing the generic arg resolving logic.
- The compiler class now takes a compile_config object and now stores the sources in the class itself
  rather than through the compile function, this may be extra useful later but for now its just for
  simplicity.
- Identifier ast nodes are now much more complex as they are able to store much more than just variable
  declarations, they can now be functions (TODO), types, and enum identifiers, which will allow static
  member access and enum option access later.
- The name resolver now resolves all different types of identifiers correctly, in the order of
  variable > func (not implemented yet) > type > enum.
- Adding support for enum instance comparison in the type checker.
- Added enum support to the interpreter.

### 31-10-2025
- Added a new enum_option_type to represent enum options which prevents ambiguity and stops chained
  option access like Enum.Option.Option.
- Converted enum_option to an actual AST node which allows for better error reporting and seperation
  of concerns from the enum declaration itself.
- The declaration collector now ensures that enums do not have multiple options with the same
  identifier.
- Variables with unknown type assignments no longer print an error as that error should be picked
  up elsewhere and this just clutters the output from testing.

### 01-11-2025
- Removed the enum_option_type as it caused more bugs than it solved. Instead, enum_type now
  contains a simple flag that denotes if its an option or the actual declaration.
- Fixed an interpreter bug where it was pushing extra scopes when entering non-standalone
  code block nodes, it now checks if its a body node before pushing a new scope.
- Fixed a few bugs in the is_valid_conversion method and also allowed implicit conversions
  when returning values that are a different type as the function for example returning an
  int from a float function.

### 02-11-2025
- Generic functions and types now use ! to denote generic parameters as otherwise it becomes
  ambiguous with less/greater than operators.
- Added a synchronize function into the parser to allow for better error recovery and continued
  parsing after an error occurs.
- Removed the old c-style cast parsing and replaced it with as-cast expressions using the new 'as'
  keyword, this is much clearer and takes inspiration from Rust casting.
- Added a default_initialized method to the init checker to check if a type can be default initialized.
  This allows for arrays to be default initialized correctly.
- Added support for default initialization of arrays in the interpreter.

### 09-11-2025
- Variable and parameter declarations now use <identifier>: <type> syntax like members for consistency.

### 14-11-2025
- Made a source file for compilation_pass to reduce clutter in the header.
- Added an advice struct to represent advice messages, these advice messages are stored in the error
  object and printed after the error message to give additional information on how to fix the error.

### 15-11-2025
- Begun working on the codegen.
- Created an llvm_type_converter for lowering data types to llvm types.

### 22-11-2025
- Heavy codegen improvements.
- Added support for codegen of:
	- Functions
	- Variable declarations
	- Basic expressions
	- If statements
	- Return statements
- Switched entry point from 'Main' to 'main' to prevent entry point case sensitivity issues with LLVM.
- Renamed 'callee' to 'caller_obj' in function_call since callee is actually not correct terminology.

### 23-11-2025
- Added a helper function into the codegen_visitor that returns an llvm::Value* directly from accepting
  an AST node, which is much safer and cleaner than using the result variable, which may still contain
  an old value from previous visits.
- Created a name_mangler class which I will use later on to mangle function names, for now its unused.
- Created a variable class to store additional information on variables such as their type, alloca slot,
  and name, this will be returned by lookup_local instead of directly returning the AllocaInst*, it also
  creates an lvalue instance upon creation, which means it doesnt need to be recreated.

### 24-11-2025
- Added for loop codegen support.
- Added unary expression codegen support.