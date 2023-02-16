# Hello Emscripten

Let's start by writing the classic C introductory program:

```C
// main.c

#include <stdio.h>

int main() {
    printf("Hello, emcc!\n");
    return 0;
}
```

Compiling and running this on our native platform we get the expected
print out:

```Shell
> clang main.c -o hello
> ./hello
Hello, emcc!
```

In order to get this working in emscripten we need a little bit of
setup. First, let us make a static directory for our webserver to
host.

```Shell
> mkdir static
```

Now we can compile our code with `emcc`. Note that `emcc` has three
different output formats: HTML, JavaScript, and WebAssembly.

Compiling to WebAssembly creates a raw `.wasm` file. Compiling to
JavaScript
adds a `.js` file containing a Module object that binds native
functionality to
the corresponding web features: e.g. stdout is mapped to
`console.log`. Finally, compiling to HTML adds an `.html` file that
provides several features for testing: a canvas for graphical
windows, an on page console shell, etc.

Since we are aiming to create
a fully functioning web app, we will be making our own HTML file and
compiling to JavaScript.

```Shell
emcc main.c -o static/hello.js
```

Then we add a bare bones `static/index.html` file to load our compiled web
app:

```HTML
<!-- static/index.html -->

<html>
<script src="hello.js"></script>
</html>
```

Looking in the static directory, we should now have our `index.html`
file, as well as two `emcc` generated files: `hello.js` and
`hello.wasm`.
To test our web app we just need a web server to host it. I will be
using the following Go web server:

```Go
// server.go

package main

import (
    "log"
    "net/http"
)

func main() {
    http.HandleFunc(
        "/",
        func(w http.ResponseWriter, r *http.Request) {
            http.ServeFile(w, r, "static/"+r.URL.Path[1:])
        })

    log.Fatal(http.ListenAndServe(":8081", nil))
}
```

Now we can run our server:

```Shell
> go run server.go
```

Visiting [localhost:8081](http://localhost:8081) in a web browser
will produce... a blank screen. But, if we open the developer
console, we see the expected output!

## Configuring the Emscripten Module

What if we wanted to change where stdout goes? Well, all we need to
do is take a look at `hello.js`. The comment at the top of the file
explains that the emscripten bindings are controlled via a `Module`
object. The `Module` object can either be created and customized
externally
before the `hello.js` script is loaded, or a default Module will be
created. At the bottom of the file, a function `run` is defined and
called (as long `Module['noInitialRun']` is not true).

Currently we are using the defualt `Module`. Looking through
`hello.js` a
bit more we can find the spot where stdout behaviour is defined:

```JavaScript
// hello.js | line 475

var out = Module['print'] || console.log.bind(console);
var err = Module['printErr'] || console.warn.bind(console);
```

If we want to set our own output, we can simply define the `Module`
object and set `Moduel['print']` to whatever we want. Lets try it:


```HTML
<!-- static/index.html -->

<html>
<script>
var Module = {
    print: window.alert.bind(window),
};
</script>
<script src="hello.js"></script>
</html>
```

Now if we run the web server and open the page we get an annoying
alert box with our message. It is probably best to switch print
back to `console.log` if you countinue using this project
as a base for the following examples.
