# Hello, World!

This quick guide walks you through creating and running a simple **Hello, World!** program using the Steel programming language. By the end, you’ll have a working project, a compiled executable, and a basic understanding of the workflow.

## Prerequisites

- Steel toolchain installed
- A terminal and your preferred editor

Verify your install:

```bash
steel --version
```

You should see a semantic version like `steel 0.1.0`.

## Creating the project

Begin by setting up your project folder with the following layout.

```
MyProject/
  src/
    main.st
  my_project.stproj
```

- **MyProject/** is the root directory of your project.
- **src/** contains all of your source code files.
- **main.st** will hold your program's entry point function.
- **my_project.stproj** is the project file used by the steel compiler.

Once you've created all the necessary files/folders, copy and paste the following into your .stproj file:

```
name = "my project"
version = "0.1.0"
type = "executable"

sources = ["src/main.st"]
```

You do not need to know what this means for now, but it is required for compilation to succeed.

*Note: If you put your ***main.st*** file in a different location, make sure to change the path in the ***sources*** array.*

## Creating the entry point

Open up your **main.st** file in your preferred code editor and enter the following:

```
func main() -> int {
  return 0;
}
```

This function will be the entry point of your program. It is very important that the function takes no parameters, and returns an int. It also **must** be called 'main' (caps sensitive) or else linking will fail.

## Printing to the screen

Currently the program doesn't do anything, to print "Hello, World!" to the console, we can use the utility function `print`.

Add the following line into your `main` function:

```
func main() -> int {
  print("Hello, World!\n");
  
  return 0;
}
```

## Building and testing the project#

Now we have our project setup and our entry point defined, it's time to build and run the project.

Open up a terminal and run the following command:

```
steelc <path-to-your-stproj>
```

This will invoke the steel compiler to build your project and produce a final executable. The location of the executable will be in a build/ directory created within the directory of the project file.

## Next steps

- Learn the basics → [Syntax](#/syntax)
- Explore common types → [Types](#/syntax-types)
- Work with projects → [CLI](#/cli)