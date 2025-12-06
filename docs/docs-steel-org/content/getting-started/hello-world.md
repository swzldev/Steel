# Hello, World!

This quick guide walks you through creating and running a simple **Hello, World!** program using the Steel programming language. By the end, you’ll have a working project, a compiled executable, and a basic understanding of the workflow.

## Prerequisites

- Steel toolchain installed
- A terminal and your preferred editor

Verify your install:

```bash
steelc --version
```

You should see a version like `steelc version 0.4.0`.

## Creating the project

The steel compiler provides a useful project utility for managing projects, to create a basic scaffold, open a terminal in the directory you want to store your project and type the following command:

```bash
steelc project new <project name>
```

If everything succeeds, you should have a new folder that looks something like this:

```
<project name>/
  src/
    main.st
  <project name>.stproj
```

- **<my project>/** is the root directory of your project.
- **src/** contains all of your source code files.
- **main.st** will hold your program's entry point function.
- **<my project>.stproj** is the project file used by the steel compiler.

## The entry point

The entry point is the function invoked at the beginning of your program's execution. Steel will have automatically generated your entry point when you created your project, open up the **src/main.st** file in your preferred code editor. It should look something like this:

```
func main() -> int {
  // Program logic goes here
  return 0;
}
```

Here we declare a standard function named **main** that takes no parameters and returns an **int**. 

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