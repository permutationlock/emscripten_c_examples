#include <stdio.h>

int main() {
    char buffer[32] = { 0 };
    FILE* f = fopen("files/hello.txt", "r");

    fread(buffer, sizeof(char), 31, f);
    printf("File contained: %s\n", buffer);

    return 0;
}
