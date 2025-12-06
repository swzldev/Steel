# Language Syntax

This page highlights the core syntax and semantics.

## Variables

```steel
let x = 42;           // Mutable variable of type 'int'.
const PI = 3.14159;   // Immutable constant of type 'float'.
```

## Functions

```steel
func add(a: int, b: int) -> int {
  return a + b;
}
```

## Control Flow

```steel
if (x > 0) {
  print("positive\n");
}
else {
  print("non-positive\n");
}

for i in 0..10 {
  print(i);
}
```

## Modules

```steel
mod math;
use math::add;
```
