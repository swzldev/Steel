# STEELC CHANGELOG

18-09-25
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