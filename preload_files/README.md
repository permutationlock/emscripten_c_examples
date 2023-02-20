# Preloading files

A simple emscripten file input example.

To build:

```bash
emcc main.c -o static/hello.js --preload-file "files/hello.txt"
```

To run server:

```bash
go run server.go
```

Visit [localhost:8081](http://localhost:8081/) and open the developer
console to see the output.
