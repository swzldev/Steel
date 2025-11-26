<h1 align="left">Steel<br/><img src=".\assets\logo-red-png.png" alt="Logo" align="right" height="230px"></h1>
<p align="left">
  <img src="https://img.shields.io/badge/latest_release-0.1.0--alpha.1-red" alt="Downloads">
  <img src="https://img.shields.io/github/stars/swzldev/Steel" alt="Stars">
</p>
<p>Steel is a performant, compiled programming language that aims to wipe out all bugs at compile time, leaving your program error free and fast.</p>
<p>
  <a href="#features">Features</a> &bull;
  <a href="#getting-started">Prerequisites</a> &bull;
  <a href="#getting-started">Getting Started</a> &bull;
  <a href="#changelog">Changelog</a> &bull;
  <a href="#credits">Credits</a>
</p>

<h2>Features</h2>
<ul>
<li>Simple and easy to write syntax inspired by languages like C, Rust, Swift and more.</li>
<li>Informative error and warning system to debug code easily.</li>
<li>Comprehensive documentation on all aspects of the language.</li>
<li>VS Code extension to allow for syntax highlighting and quick code compilation. (In development)</li>
</ul>

<h2>Prerequisites</h2>
<p>Steel uses LLVM as its backend, therefore you will need to have it installed for building to succeed. To install LLVM, head to the releases page on the <a href="https://github.com/llvm/llvm-project/releases/">github repo</a> and download the latest release of LLVM+Clang for your target platform, or optionally, build it from source.</p>

<h2>Getting started</h2>
<p>Before you start, make sure you have completed all the <a href="#prerequisites">prerequisites</a> listed above.</p>

<p>To start writing code, first download the <a href="https://github.com/swzldev/Steel/releases/">latest release</a> of the compiler. This is nescessary for converting your Steel code into a functional executable. It is highly reccomended that you add the steelc executable to your system PATH, as it will make it much easier to compile projects located anywhere on your system.</p>

<p>After downloading the compiler, you can begin to create your first project. Once your project folder is setup, the first step is to create a project file, this is used to configure different project settings, such as build type, name, etc, as well as which files are part of the project.</p>

<p>Steel project files use TOML, and have the *.stproj extension. You can use the template below to create yours:</p>

<pre><code>name = "MyProject"
version = "1.0.0"
type = "executable"

sources = ["Main.s"]
</code></pre>

<p>Here we gave it one source file: 'Main.s', which does not yet exist. Note that each source file is a path relative to the project file, therefore if you want to keep your source files within a source folder, you need to write it as part of the source path, e.g. 'src/Main.s'. Make a 'Main.s' file in the same directory, this will be the file for your entry point. The entry point must be called 'main' and return an int:</p>

<pre><code>func main() -> int {
    printf("Hello, World!");
    return 0;
}
</code></pre>

<p>To compile your project, open a terminal in the project directory and run the following command (make sure to change 'project.stproj' to the name/relative path of your project file if it differs):</p>

<code>steelc "project.stproj"</code>

<p>Assuming you did everything correctly, you should now see a 'build' folder in your project directory, with your final executable inside, the filename of the executable will be the same as the stproj filename (with the .stproj extension truncated).</p>
<p><i>Note: If you did not add steelc to your system PATH, you will need to run the command from the directory where steelc.exe is located, or provide the full path to the steelc executable.</i></p>

<h3>Important notes</h3>
<p>Steel is currently in alpha, this means many features are not fully functional and may contain plenty of bugs, this version is not meant to be released publicly.</p>

<h2>Changelog</h2>
<p>To view the full changelog, click <a href="./steelc/changelog.md">here</a>.</p>

<h2>Credits</h2>
</p>As of right now I am the sole developer of Steel.</p>

<h2>License</h2>
<p>This project is currently liscensed under the MIT license. However, this code is not intended to be used outside of this project, and may require heavy changes if you intend to use it yourself.</p>
