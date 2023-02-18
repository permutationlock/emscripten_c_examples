# Clang WASM Example

Here is a simple example of how to target `wasm32` with clang.

To build:

```bash
clang fibonacci.c -o static/fibonacci.wasm --target=wasm32 --no-standard-libraries -Wl,--export-all -Wl,--no-entry
```

To run server:

```bash
go run server.go
```

Visit [localhost:8081](http://localhost:8081/) and open the developer
console to see the output.
