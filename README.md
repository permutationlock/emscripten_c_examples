# Cross-compiling C with Emscripten

In this project we look at how to use [Emscripten][6] to make C
applications compile to run in a web page. First, we'll get a basic
"Hello, world!"
working and look at how to tell the browser what to do with
stdout and stderr. Then we'll look at getting more general
file i/o working on the web. Finally, we'll get a full graphical app
working with [Raylib][1], matching native behavior as close as possible.

In order to follow along you will first need a C
compiler; I will use [Clang][5], but feel free to use whatever you
are comfortable with. Second, you will obviously need to have
[Emscripten][6] installed. You will also need a web server to test
your web builds; I will use [Go][7] and provide a simple server, but feel
free to use what you are comfortable with. I'll also use [make][3] to
build projects and [git][8] to pull source code.

All shell commands will be written assuming a Unix-like system.

This repository was adapted from an article, each directory
corresponds to the end result of an example from the article. The
`README.md` file in each directory contains the corresponding article
section. The order of sections is as follows:

 1. [hello\_emcc](https://github.com/permutationlock/emscripten_c_examples/tree/main/hello_emcc)
 2. [file\_io](https://github.com/permutationlock/emscripten_c_examples/tree/main/file_io)
 3. [resizable\_raylib](https://github.com/permutationlock/emscripten_c_examples/tree/main/resizable_raylib)

[1]: https://github.com/raysan5/raylib
[3]: https://www.gnu.org/software/make/
[5]: https://clang.llvm.org/
[6]: https://emscripten.org/
[7]: https://go.dev/
[8]: https://git-scm.com/

