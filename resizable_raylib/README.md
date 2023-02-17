# A resizable Raylib app

Porting native command line applications to the web is clearly not
that useful, so let's do something a bit more complicated. The goal
of this section is to create a resizable window drawing some centered
text. On native this will be a standard window in whatever window
manager is being used, e.g. X. On the web our "window" will be a
canvas that fills the content area of the browser window and resizes
with it.

Start once again with the hello emcc example from the first section.
For this project we will need to download the Raylib source code:

```Shell
> git clone https://github.com/raysan5/raylib.git
```

## Setup

To compile Raylib for your local system you will need to follow the
instructions found on the [GitHub readme][1]. The easiest way to get
Raylib working is to use a package manager to install the library and
its dependencies. Local compilation examples below will assume that
the Raylib libraries are already installed.

The main focus of this section is getting
Raylib apps working via emscripten which, thankfully, does not require
any dependencies other than the Raylib source and the `emcc`
compiler.

The Raylib Makefile contains the option to target web platforms. We
will use the Makefile to build an emscripten compatible library file
as follows:

```Shell
> mkdir libweb
> cd raylib/src/
> PLATFORM=PLATFORM_WEB make
> cp libraylib.a ../../libweb/
> cd ../../
```

It will also be nice to grab the Raylib header file and put it in a
dedicated include directory:

```Shell
> mkdir include
> cp raylib/src/raylib.h include/
```

## Drawing a square window

Next let us modify main.c to have it create a 300 pixel square Raylib
window and fill it with a dark gray background:

```c
// main.c

#include <raylib.h>

int main() {
    InitWindow(300, 300, "hello, raylib!");

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);
        EndDrawing();
    }

    return 0;
}
```

Our build commands in this section will get fairly involved, so we
will head things off by making a [makefile][3] (feel free to replace
this with your build script of choice).

```shell
# makefile

LOAD = -lraylib
INCLUDE = -Iinclude

CC = clang
LIB =
FLAGS =

EMCC = EMCC
WLIB = -Llibweb
WFLAGS = -s USE_GLFW=3 -s ASYNCIFY

all: native web

native: main.c
	$(CC) main.c -o hello $(FLAGS) $(LIB) $(INCLUDE) $(LOAD) 

web: main.c
	$(EMCC) main.c -o static/hello.js $(WFLAGS) $(WLIB) $(INCLUDE) $(LOAD) 

clean:
	rm hello static/hello.js static/hello.wasm
```

The commands produced by this makefile are:

```shell
> clang main.c -o hello -Iinclude -lraylib
> emcc main.c -o static/hello.js -Iinclude -Llibweb \
>     -lraylib -s USE_GLFW=3 -s ASYNCIFY
```

The native build command assumes that Raylib is already
in the main build path, i.e. installed via a package manager. You
could modify the lib variable in the makefile to match your setup.

The `-s USE_GLFW=3` flag is necessary as the graphics code from
Raylib will utilize [gflw][2]. The `-s ASYNCIFY` tells the compiler
to modify our code to allow it to interact with asynchronous
JavaScript. Currently our code is an infinite loop, which would block
out all JavaScript events if taken literally. Both flags are
necessary in our case because the Raylib library file requires them.

You should now be able to run `make` to build for both native and
web. Testing on web we now get the following error:

```Shell
Uncaught (in promise) TypeError: Module.canvas is undefined
```

Emscripten requires an HTML canvas to draw a graphical window. If we
look through `staic/hello.js` we can find the relevant property is
`Module['canvas']`. Let us modify our HTML file to add a canvas
element:

```HTML
<!-- static/hello.js -->
<html>
    <body>
        <canvas id="canvas"></canvas>
    </body>
    <script>
        var Module = {
            canvas: document.getElementById('canvas'),
        };
    </script>
    <script src="hello.js"></script>
<html>
```

Refreshing the page, you should now see a 300 pixel gray square.

## Using the Emscripten animation loop

Currently our code on both native and the web runs a simple infinite
loop. On the web it is much better to run an "asynchronous loop,"
that is, define a function that contains the body of the loop and
have the browser regularly call that function to request animation
frames. To do this we will modify our code as follows:

```c
// main.c

#include <raylib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void update() {
    BeginDrawing();
    ClearBackground(DARKGRAY);
    EndDrawing();
}

int main() {
    InitWindow(300, 300, "hello, raylib!");

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(update, 0, 1);
#else
    while(!WindowShouldClose()) {
        update();
    }
#endif

    return 0;
}
```

The `#ifdef __EMSCRIPTEN__` preprocessor condition allows us to check
whether the `emcc` compiler is being used. If the compiler is `emcc`,
we set `update` as the emscripten main loop callback function.
Otherwise we simply run the animation loop as normal.

## Making the window resizable

In order to make a window resizable in a native Raylib application,
we simply need to add the following line just before calling
`InitWindow`:

```C
// main.c

// ...

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(300, 300, "hello, raylib!");

// ...
```

Let us also make the `update` function draw some centered text so we
can make sure that resizing is working correctly:

```C
// main.c

// ...

void update() {
    char* text = "Hello, emcc!";

    BeginDrawing();

    ClearBackground(DARKGRAY);

    int width = GetScreenWidth();
    int height = GetScreenHeight();

    int twidth = MeasureText(text, 22);
    int theight = 10;

    DrawText(
        text,
        (width - twidth) / 2,
        (height - theight) / 2,
        22,
        RAYWHITE
    );

    EndDrawing();
}

// ...
```

Making and re-running both the native and web builds, we should see
that the native window is now resizable. Unfortunately, the web app
remains unchanged.

The first
issue is that the "window" in the context of the
web application is the canvas, not the browser window. Unfortunately,
simply making the canvas resize with the page does not fix the
problem as
Emscripten will still not emulate the canvas resizing as a native
window resize.

The solution is to do things manually by utilizing another feature of
Emscripten: [cwrap][4]. We will first need to create a C function
that should be called whenever the canvas is resized:

```C
// main.c

// ...

#ifdef __EMSCRIPTEN__
extern void on_resize(int width, int height) {
    SetWindowSize(width, height);
}
#endif

// ...
```

Then we will add the following compile flags to our Makefile to tell
`emcc` to export our new `on_resize` function in addition to `main`:

```Shell
# Makefile

# ...

WFLAGS = -s USE_GLFW=3 -s ASYNCIFY \
         -s EXPORTED_RUNTIME_METHODS=cwrap \
         -s EXPORTED_FUNCTIONS=_main,_on_resize

# ...
```

We also tell `emcc` to generate `cwrap` functionality which allows us
to wrap exported C functions as JavaScript functions.

If we call `make` with our updated `Makefile` and look through
`static/hello.js` we can find the defintion of `cwrap`:

```JavaScript
// static/hello.js | line 8451
/**
 * @param {string=} returnType
 * @param {Array=} argTypes
 * @param {Object=} opts
 */
function cwrap(ident, returnType, argTypes, opts) {
  return function() {
    return ccall(ident, returnType, argTypes, arguments, opts);
  }
}
```

Not delving too much into the inner workings,
`Module.cwrap` requires three
arguments: `ident` is the name of the exported C function to
be wrapped, `returnType` is the return type, and `argTypes` is an
array indicating the function arguments. The JavaScript equivalent of
our function `void on_resize(int, int)` is
`on_resize(number,number)`. Therefore `ident` should
be `on_resize`, `returnType` should be `null`, and `argTypes` should
be `[number, number]`.

To put our newly exported function into use with `cwrap` we need to
make some major modifications to `static/index.html`:

```HTML
<!-- static/index.html -->

<html>
    <body>
        <canvas id="canvas"></canvas>
    </body>
    <script>
        function on_load() {
            let canvas = document.getElementById(
                "canvas"
            );
            let on_resize = Module.cwrap(
                "on_resize",
                null,
                ["number", "number"]
            );
            let resize_handler = () => {
                const w = canvas.width
                    = window.innerWidth;
                const h = canvas.height
                    = window.innerHeight;
                on_resize(w, h);
            }
            window.addEventListener(
                "resize",
                resize_handler,
                true
            );
            resize_handler();
        }

        var Module = {
            postRun: [on_load],
            canvas: document.getElementById('canvas'),
        };
    </script>
    <script async src="hello.js"></script>
</html>
```

The above changes make it so that
the canvas resizes with the browser window and the `on_resize`
function is called when the canvas is
resized. The `postRun` property of `Module` is an array of functions
to be called after the Emscripten module is running.
We need our `on_load` function to be called after `Module` has been
configured because we require the `Module.cwrap` function.

Recompiling, everything should work as expected and the window should
resize on web!

If you want to have the canvas completely fill the
browser window and eliminate any white edges, simply add the
following header tag to `static/index.html`:

```HTML
<!-- static/index.html -->

<!-- ... -->

    <head>
        <title>Hello, emcc!</title>
        <style>
            * {
                padding: 0;
                margin: 0;
            }
            body {
                overflow: hidden;
            }
        </style>
    </head>

<!-- ... -->
```

Our complete files at the end should look as follows:

```C
// main.c

#include <raylib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __EMSCRIPTEN__
extern void on_resize(int width, int height) {
    SetWindowSize(width, height);
}
#endif

void update() {
    char* text = "Hello, emcc!";

    BeginDrawing();

    ClearBackground(DARKGRAY);

    int width = GetScreenWidth();
    int height = GetScreenHeight();

    int twidth = MeasureText(text, 22);
    int theight = 10;

    DrawText(
        text,
        (width - twidth) / 2,
        (height - theight) / 2,
        22,
        RAYWHITE
    );

    EndDrawing();
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(300, 300, "hello, raylib!");

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(update, 0, 1);
#else
    while(!WindowShouldClose()) {
        update();
    }
#endif

    return 0;
}
```

```HTML
<!-- static/index.html -->

<html>
    <head>
        <title>Hello emscripten</title>
        <style>
            * {
                padding: 0;
                margin: 0;
            }
            body {
                overflow: hidden;
            }
        </style>
    </head>
    <body>
        <canvas id="canvas"></canvas>
    </body>
    <script>
        function on_load() {
            let canvas = document.getElementById(
                "canvas"
            );
            let on_resize = Module.cwrap(
                "on_resize",
                null,
                ["number", "number"]
            );
            let resize_handler = () => {
                const w = canvas.width
                    = window.innerWidth;
                const h = canvas.height
                    = window.innerHeight;
                on_resize(w, h);
            }
            window.addEventListener(
                "resize",
                resize_handler,
                true
            );
            resize_handler();
        }

        var Module = {
            postRun: [on_load],
            canvas: document.getElementById('canvas'),
        };
    </script>
    <script async src="hello.js"></script>
</html>
```

```Shell
# Makefile

LOAD = -lraylib
INCLUDE = -Iinclude

CC = clang
LIB =
FLAGS =

EMCC = emcc
WLIB = -Llibweb
WFLAGS = -s USE_GLFW=3 -s ASYNCIFY \
         -s EXPORTED_RUNTIME_METHODS=cwrap \
         -s EXPORTED_FUNCTIONS=_main,_on_resize

all: native web

native: main.c
	$(CC) main.c -o hello $(FLAGS) $(LIB) $(INCLUDE) $(LOAD) 

web: main.c
	$(EMCC) main.c -o static/hello.js $(WFLAGS) $(WLIB) $(INCLUDE) $(LOAD) 

clean:
	rm hello static/hello.js static/hello.wasm
```

[1]: https://github.com/raysan5/raylib
[2]: https://www.glfw.org/
[3]: https://www.gnu.org/software/make/
[4]: https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html?highlight=cwrap
