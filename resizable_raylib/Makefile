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
