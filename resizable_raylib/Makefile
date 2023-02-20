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
	$(CC) main.c -o raysize $(FLAGS) $(LIB) $(INCLUDE) $(LOAD) 

web: main.c
	$(EMCC) main.c -o static/raysize.js $(WFLAGS) $(WLIB) $(INCLUDE) $(LOAD) 

clean:
	rm raysize static/raysize.js static/raysize.wasm
