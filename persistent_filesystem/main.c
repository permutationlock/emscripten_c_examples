#include <stdio.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void print_file(const char* fpath) {
    char buffer[32] = { 0 };
    FILE* f = fopen(fpath, "r");

    while(fread(buffer, sizeof(char), 31, f)) {
        printf("%s", buffer);
        memset((void*)buffer, 0, 31);
    }

    fclose(f);
}

void append_file(const char* fpath, const char* s) {
    FILE* f = fopen(fpath, "a");
    fputs(s, f);
    fclose(f);
}

void file_test() {
    const char* file = "files/count.txt";
    char s[16] = "I was here :)\n";
    append_file(file, s);
    print_file(file);

#ifdef __EMSCRIPTEN__
    EM_ASM(
        FS.syncfs((err) => {});
    );
#endif
}

int main() {
#ifdef __EMSCRIPTEN__
    EM_ASM(
        FS.mkdir('files');
        FS.mount(IDBFS, {}, 'files');
        FS.syncfs(true, (err) => {
            assert(!err);
            ccall('file_test', 'v');
        });
    );

    emscripten_exit_with_live_runtime();
#else
    file_test();
#endif

    return 0;
}
