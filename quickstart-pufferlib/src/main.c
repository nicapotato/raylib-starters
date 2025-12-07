#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir
#include "connect4/connect4_app.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// --- Helper Functions ---

void InitApp() {
    // Set up resource directory
    SearchAndSetResourceDir("resources");
    
    // Initialize Connect4 Game (Default to AI vs AI mode)
    InitConnect4(0);
}

int main()
{
    // Use high DPI if available (looks much sharper on Mac)
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    
    // Updated dimensions to 480x800 for mobile-friendly aspect ratio
    InitWindow(480, 800, "Raylib Quickstart C - Connect4");
    InitAudioDevice();
    
    InitApp();

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Update
        UpdateConnect4();

        // Draw
        BeginDrawing();
            DrawConnect4();
        EndDrawing();
    }

    // Cleanup
    UnloadConnect4();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
