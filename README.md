<h1 align="left">Steel<br/><img src=".\assets\logo-red-png.png" alt="Logo" align="right" height="230px"></h1>
<p align="left">
    <img src="https://img.shields.io/badge/latest_version-0.5.0--alpha-yellow" alt="Downloads">
    <img src="https://img.shields.io/github/stars/swzldev/Steel" alt="Stars">
</p>
<p>Steel is a performant, compiled programming language that aims to wipe out all bugs at compile time, leaving your program error free and fast.</p>

<p>
    <a href="#features">Features</a> &bull;
    <a href="#installation">Installation</a> &bull;
    <a href="#getting-started">Getting Started</a> &bull;
    <a href="#changelog">Changelog</a> &bull;
    <a href="#credits">Credits</a>
</p>

> <h3>Important note:</h3>
> <p>Steel is currently in alpha, this means many features are not fully functional and may contain plenty of bugs, this version is not meant to be released publicly.</p>

<h2>Features</h2>
<ul>
    <li>Simple and easy to write syntax inspired by languages like C, Rust, Swift and more.</li>
    <li>Informative error and warning system to debug code easily.</li>
    <li>Comprehensive documentation on all aspects of the language.</li>
    <li>VS Code extension to allow for syntax highlighting and quick code compilation. (In development)</li>
</ul>

<h2>Installation</h2>
<p>To install the steel toolchain, first download the latest release of the compiler for your platform in the <a href="https://github.com/swzldev/Steel/releases/">releases</a> page.</p>

<p>After downloading, extract the contents of the zip file to a folder of your choice. It is <strong>highly</strong> recommended to add the folder containing the steelc executable to your system PATH, as some tools (like the Steel VS Code extension) wont function if if you don't. Additionally, it will make compiling Steel code from anywhere on your system much easier.</p>

<h2>Getting started</h2>
<p>Before you start, make sure the steel toolchain is installed on your system (see <a href="#installation">installation</a>). In this short guide, it is assumed you chose to add steelc to your PATH. If you havn't, you may need to specify the explicit location of the steelc executable when following along with any commands in this tutorial.</p>

<p>To begin, we first need to setup a steel project. The steelc tool provides a few useful commands that we will use to create our project.</p>

<p>Open a terminal in the directory you want to create your project in, and run the following command:</p>

<code>steelc project new "&lt;project-name&gt;"</code>

<p><i>Replace &lt;project-name&gt; with the name you want to give your project.</i></p>

<p>This will create a new folder with the project name, and setup a basic steel project structure inside it. It should look something like this:</p>

<pre><code>&lt;project-name&gt;/
&#x251C;&#x2500;&#x2500; src/
&#x2502;   &#x2514;&#x2500; main.st
&#x2514;&#x2500; &lt;project-name&gt;.stproj
</code></pre>

<p>Open up the 'src/main.st' file in your preferred code editor, and replace its content with the following:</p>

<pre><code>func main() -> int {
    printf("Hello, Steel!\n");
    return 0;
}
</code></pre>

<p>Now let's build our project. Open up a terminal in the project folder and run the following command:</p>

<code>steelc build <project-name>.stproj</code>

<p><i>Replace &lt;project-name&gt; with the name you gave your project.</i></p>

<p>If everything builds successfully, you should now see a 'build' folder in your project directory. Open it up and run the executable inside, you should see "Hello, Steel!" displayed to the console.</p>

<h2>Changelog</h2>
<p>To view the full changelog, click <a href="./steelc/changelog.md">here</a>.</p>

<h2>Credits</h2>
<p>Developers:</p>
<ul>
    <li>swzldev</li>
</ul>

<h2>License</h2>
<p>This project is currently liscensed under the MIT license. However, this code is not intended to be used outside of this project, and may require heavy changes if you intend to use it yourself.</p>
