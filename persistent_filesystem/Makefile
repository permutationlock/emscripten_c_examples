CC = clang
FLAGS =

EMCC = emcc
WFLAGS = -lidbfs.js \
         -s EXPORTED_RUNTIME_METHODS=ccall \
         -s EXPORTED_FUNCTIONS=_main,_file_test

all: native web

native: main.c
	$(CC) main.c -o pfiles $(FLAGS)

web: main.c
	$(EMCC) main.c -o static/pfiles.js $(WFLAGS)

clean:
	rm pfiles static/pfiles.js static/pfiles.wasm
