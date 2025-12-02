# Install Steel

Install the Steel toolchain and verify your setup.

## Requirements

- Windows, macOS, or Linux
- Recent terminal and a code editor

## Installation

```bash
curl -fsSL https://get.steel.sh | sh
```

Or download from the official releases page and add `steel` to your `PATH`.

## Verify

```bash
steel --version
```

Expected output is a semantic version like `steel 0.1.0`.

## Hello, Steel

```steel
fn main() {
  print("Hello, Steel!\n");
}
```

Run it:

```bash
steel run main.st
```
