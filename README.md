# Cross-compiling C with Emscripten

In this project we look at how to use emscripten to make C
applications compile to run in a web page. First, we'll get a basic
"Hello, world!"
working and look at how to tell the browser what to do with
stdout and stderr. Then we'll look at getting more general
file i/o working on the web. Finally, we'll get a full graphical app
working with Raylib, matching native behavior as much as possible.

In order to follow along you will first need a C
compiler; I will use Clang, but feel free to use GCC or whatever you
are comfortable with. Second, you will obviously need to have
Emscripten installed. You will also need a web server to test
your web builds; I will use Go and provide a simple server, but feel
free to use what you are comfortable with. I'll also use make to
build projects and git to pull source code.

All shell commands will be written assuming a Unix-like system.
