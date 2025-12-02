# Language Syntax

This page highlights the core syntax and semantics.

## Variables

```steel
let x = 42;
const PI = 3.14159;
```

## Functions

```steel
fn add(a: int, b: int) -> int {
  return a + b;
}
```

## Control Flow

```steel
if x > 0 {
  print("positive\n");
} else {
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
