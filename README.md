<h1 align="left">Steel<br/><img src=".\assets\logo-red-png.png" alt="Logo" align="right" height="230px"></h1>
<p align="left">
  <img src="https://img.shields.io/badge/latest_release-0.1.0--alpha.1-red" alt="Downloads">
  <img src="https://img.shields.io/github/stars/swzldev/Steel" alt="Stars">
</p>
<p>Steel is a performant, compiled programming language that aims to wipe out all bugs at compile time, leaving your program error free and fast.</p>
<p>
  <a href="#features">Features</a> &bull;
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

<h2>Getting started</h2>
<p>To begin first download the <a href="https://github.com/swzldev/Steel/releases/">latest release</a> of the compiler. This is nescessary for converting your Steel code into a functional executable.</p>

<p>After downloading the compiler, you can begin to create your first Steel project. Steel projects use TOML, and use the *.stproj extension. You can use the template below to create your project:</p>

<code>
  name = "MyProject"
  version = "1.0.0"
  type = "executable"

  sources = ["Main.s"]
</code>

<p>Now, make a "Main.s" file in the same directory, this will be the file for your entry point. The entry point must be called "main" and return an int:</p>

<code>
  func main() -> int {
      printf("Hello, World!");
      return 0;
  }
</code>

<p>Now, to compile your project, open a terminal in the project directory and run the following command:</p>
<code>
  steelc "<path to your .stproj file>"
</code>
<p>Assuming you did everything correctly, you should now see a 'build' folder in your project directory, inside of it will be your executable.</p>

<h3>Important notes</h3>
<p>Steel is currently in pre-alpha, this means many features are not fully functional and may contain plenty of bugs, this version is not meant to be released publicly.</p>

<h2>Changelog</h2>
<p>To view the full changelog, click <a href="./steelc/changelog.md">here</a>.</p>

<h2>Credits</h2>
</p>As of right now I am the sole developer of Steel.</p>

<h2>License</h2>
<p>This project is currently liscensed under the MIT license. However, this code is not intended to be used outside of this project, and may require heavy changes if you intend to use it yourself.</p>
