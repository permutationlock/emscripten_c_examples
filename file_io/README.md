# File I/O

When running a native application you have a local file system to
use for reading and writing data. Emscripten provides a way to
emulate a somewhat limited version of this functionality.

Starting from the basic hello emcc project created above,
let us modify `main.c` as follows:

```C
// main.c

#include <stdio.h>

int main() {
    printf("Hello, emcc!\n");

    char buffer[32] = { 0 };
    FILE* f = fopen("static/hello.txt", "r");
    fread(buffer, sizeof(char), 31, f);

    printf("File contained: %s\n", buffer);

    return 0;
}
```

Next let us create that `static/hello.txt` file:

```Shell
> mkdir files
> echo "I am a file!" > files/hello.txt
```

Compiling and running on our local system, we get the desired result:

```Shell
> clang main.c -o hello
> ./hello
Hello, emcc!
File contained: I am a file!

```

However, if we compile and test our file on the web, the console
shows the following error:

```Shell
Uncaught (in promise) RuntimeError: index out of bounds
    createExportWrapper http://localhost:8081/hello.js:903
    callMain http://localhost:8081/hello.js:4318
    doRun http://localhost:8081/hello.js:4371
    run http://localhost:8081/hello.js:4386
    runCaller http://localhost:8081/hello.js:4303
    removeRunDependency http://localhost:8081/hello.js:832
    receiveInstance http://localhost:8081/hello.js:991
    receiveInstantiationResult http://localhost:8081/hello.js:1009
```

We never told `emcc` about our `files/hello.txt` file, so of course the
WebAssembly app cannot find the file. In order to package a file or
directory of files to be packaged alongside our code, we need to use
a flag to tell `emcc` to include them:

```Shell
> emcc main.c -o static/hello.js --preload-file files/
```

Looking in the static directory we now see that a third generated
file has appeard: `hello.data`. Reloading our web app we should now
see the same console output that our native binary produced.

