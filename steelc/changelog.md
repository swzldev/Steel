# STEELC CHANGELOG

18-09-2025
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

19-09-2025
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