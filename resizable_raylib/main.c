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
