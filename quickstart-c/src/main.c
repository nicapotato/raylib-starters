#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// --- Global State ---
typedef enum {
    STATE_MENU,
    STATE_SHADER
} AppState;

AppState currentState = STATE_MENU;

// Menu variables
Texture2D wabbit;
Music music;
Vector2 position = { 400.0f, 200.0f };
Vector2 velocity = { 200.0f, 200.0f };
float rotation = 0.0f;

// Shader variables
Shader shader;
int resolutionLoc;
int timeLoc;
int pointerLoc;
float shaderTime = 0.0f;

// Button logic
Rectangle buttonBounds = { 50, 50, 200, 50 };
bool btnState = false; // false = normal, true = pressed

// Close button logic
Rectangle closeButtonBounds; // Will be set in Update
bool closeBtnState = false;

// --- Helper Functions ---

void InitApp() {
    // Set up resource directory
    SearchAndSetResourceDir("resources");

    // Load Menu Assets
    wabbit = LoadTexture("wabbit_alpha.png");
    // Check if music exists before loading (since we removed it from Makefile copy)
    // For this demo we'll skip music if not found or keep it if it's there.
    // Assuming standard raylib example assets might be missing if removed from Makefile
    if (FileExists("crystal_cave_track.mp3")) {
        music = LoadMusicStream("crystal_cave_track.mp3");
        PlayMusicStream(music);
    }

    // Load Shader
    // Note: We only need a fragment shader, pass 0/NULL for vertex shader
    #if defined(PLATFORM_WEB)
        shader = LoadShader(0, "wave_web.fs");
    #else
        shader = LoadShader(0, "wave.fs");
    #endif

    // Get uniform locations
    resolutionLoc = GetShaderLocation(shader, "resolution");
    timeLoc = GetShaderLocation(shader, "time");
    pointerLoc = GetShaderLocation(shader, "pointer");
}

void UpdateMenu() {
    // Music
    if (IsMusicStreamPlaying(music)) UpdateMusicStream(music);
    
    float deltaTime = GetFrameTime();
    
    // Bouncing Wabbit Logic
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
    rotation += 90.0f * deltaTime;
    
    if (position.x <= 0 || position.x >= GetScreenWidth() - wabbit.width) velocity.x *= -1;
    if (position.y <= 0 || position.y >= GetScreenHeight() - wabbit.height) velocity.y *= -1;

    // Button Logic
    Vector2 mousePoint = GetMousePosition();
    btnState = false;
    
    if (CheckCollisionPointRec(mousePoint, buttonBounds)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) btnState = true;
        
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            currentState = STATE_SHADER;
        }
    }
}

void DrawMenu() {
    ClearBackground(RAYWHITE);

    // Draw Wabbit
    Rectangle source = { 0, 0, (float)wabbit.width, (float)wabbit.height };
    Rectangle dest = { position.x + wabbit.width/2, position.y + wabbit.height/2, (float)wabbit.width, (float)wabbit.height };
    Vector2 origin = { (float)wabbit.width/2, (float)wabbit.height/2 };
    DrawTexturePro(wabbit, source, dest, origin, rotation, WHITE);

    // Draw Button
    DrawRectangleRec(buttonBounds, btnState ? LIGHTGRAY : GRAY);
    DrawRectangleLines((int)buttonBounds.x, (int)buttonBounds.y, (int)buttonBounds.width, (int)buttonBounds.height, BLACK);
    DrawText("Interactive Shader 1", (int)buttonBounds.x + 15, (int)buttonBounds.y + 15, 20, BLACK);
    
    DrawText("Main Menu - Quickstart C", 10, 10, 20, DARKGRAY);
}

void UpdateShader() {
    // Update Time
    float deltaTime = GetFrameTime();
    shaderTime += deltaTime;
    
    // Handle scroll for time velocity
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        shaderTime += wheel * 0.5f; 
    }

    // --- Update Uniforms ---
    float resolution[2] = { (float)GetRenderWidth(), (float)GetRenderHeight() };
    SetShaderValue(shader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);

    SetShaderValue(shader, timeLoc, &shaderTime, SHADER_UNIFORM_FLOAT);

    Vector2 mouse = GetMousePosition();
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    
    // Center origin, Y-up
    float pointerX = (mouse.x - screenWidth / 2.0f) / (screenHeight / 2.0f);
    float pointerY = -(mouse.y - screenHeight / 2.0f) / (screenHeight / 2.0f);

    float pointer[2] = { pointerX, pointerY };
    SetShaderValue(shader, pointerLoc, pointer, SHADER_UNIFORM_VEC2);

    // Close Button Logic
    // Top right corner
    closeButtonBounds = (Rectangle){ (float)GetScreenWidth() - 40, 10, 30, 30 };
    Vector2 mousePoint = GetMousePosition();
    closeBtnState = false;

    if (CheckCollisionPointRec(mousePoint, closeButtonBounds)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) closeBtnState = true;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            currentState = STATE_MENU;
        }
    }
}

void DrawShader() {
    BeginShaderMode(shader);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
    EndShaderMode();

    // Draw UI on top
    // Close Button
    DrawRectangleRec(closeButtonBounds, closeBtnState ? RED : MAROON);
    DrawText("X", (int)closeButtonBounds.x + 8, (int)closeButtonBounds.y + 5, 20, WHITE);
    
    // Instructions
    DrawText("Scroll to change speed", 10, GetScreenHeight() - 30, 20, WHITE);
}

int main()
{
    // Use high DPI if available (looks much sharper on Mac)
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(1280, 800, "Raylib Quickstart C");
    InitAudioDevice();
    
    InitApp();

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Update
        switch(currentState) {
            case STATE_MENU: UpdateMenu(); break;
            case STATE_SHADER: UpdateShader(); break;
        }

        // Draw
        BeginDrawing();
            switch(currentState) {
                case STATE_MENU: DrawMenu(); break;
                case STATE_SHADER: DrawShader(); break;
            }
        EndDrawing();
    }

    // Cleanup
    UnloadTexture(wabbit);
    if (IsMusicStreamPlaying(music)) UnloadMusicStream(music);
    UnloadShader(shader);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
