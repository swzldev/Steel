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
- Fixed a bug in the type checker where it would invalidate dereferences to non
  variables instead of just checking its a pointer type. Chained pointer
  derefences now work as intended.
- Added a flag to enable/disable generics, so that early release builds do not
  have access to half finished generics functionality.