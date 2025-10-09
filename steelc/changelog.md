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

### 26-09-2025
- Added a new primitive DT_CUSTOM to easily check if a data type is custom without
  having to dynamic cast, however this is new and so currently it isnt used and
  will need to be updated throughout the codebase.
- The generic_type data type now holds a reference to the generic parameter ast node
  this means we can easily update the parameter instead of having to update all
  generics - i dont know why it wasnt like this before but generics wernt functional
  anyway.

### 27-09-2025
- Added generic support into the type and name resolver and also added an unbox_generic
  function to the type checker which hopefully works.

### 28-09-2025
- The type resolver now resolves generic function arguments.
- The type checker now scores based on generic argument matches that are explicit.
- Added a flag to the function declaration to check whether its been constrained,
  this means we dont type check the generic function body only the unboxed versions.

### 05-10-2025
- Added a new using ast<T> to reduce long type names.
- Added an is_body flag to the block_statement node (now code_block) so that the
  code block can now check itself if it should push a new scope or not.
- Updated all ast nodes to just include ast_fwd.h instead of each individual node
  header which just keeps everything a bit cleaner and easier to manage.
- Renamed if_statement's else_block to else_statement as it may not be a block.
- Removed the return_if node and moved it into the return_statement node as it was
  unnecessary to have a seperate node for it.

### 06-10-2025
- Made a new type_handle class to hold both the syntax type and the resolved type.

### 07-10-2025
- Completely rewriting the type system, data_type is no longer always a primitive
  but instead an abstract class, primitive type is its own type now. This will make
  it much easier to cast between types especially with the new as_ functions like
  as_primitive and as_custom.
- Made ast_ptr a template which reduces the writing of non root ast nodes.
  ast_node_ptr can be used as a replacement for storing root node pointers.

### 08-10-2025
- Renamed i16, i32, and i64 to short, int, and long.

### 09-10-2025
- Updated the type node to support different types like arrays and pointers.
- Added support for += and -= operators.