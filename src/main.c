#include <raylib.h>
#include <raygui.h>

int main(int argc, char** argv) {
    InitWindow(500, 500, "VOC");

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
