#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#define RAYLIB_ASEPRITE_IMPLEMENTATION
#include "raylib-aseprite.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Nick CICD Trigger II

// --- Global State ---
typedef enum {
    STATE_MENU,
    STATE_SHADER,
    STATE_ASEPRITE
} AppState;

AppState currentState = STATE_MENU;

// Menu variables
Texture2D wabbit;
Music music;
bool musicLoaded = false;
Vector2 position = { 400.0f, 200.0f };
Vector2 velocity = { 200.0f, 200.0f };
float rotation = 0.0f;

// Shader variables
Shader shader;
int resolutionLoc;
int timeLoc;
int pointerLoc;
float shaderTime = 0.0f;

// Aseprite preview (falling ball)
static Aseprite ballAseprite = { 0 };
static AsepriteTag ballTag = { 0 };
static bool ballUseTag = false;
static int ballManualFrame = 0;
static float ballManualTimer = 0.0f;

// Button logic
Rectangle buttonBounds = { 50, 50, 200, 50 };
Rectangle asepriteButtonBounds = { 50, 110, 200, 50 };
bool btnShaderState = false;
bool btnAsepriteState = false;

// Close button logic
Rectangle closeButtonBounds; // Will be set in Update
bool closeBtnState = false;

// --- Helper Functions ---

static void InitBallAseprite(void) {
    ballAseprite = LoadAseprite("anim-sprite-ball-falling.aseprite");
    assert(IsAsepriteValid(ballAseprite));

    if (GetAsepriteTagCount(ballAseprite) > 0) {
        ballTag = LoadAsepriteTagFromIndex(ballAseprite, 0);
        assert(IsAsepriteTagValid(ballTag));
        ballUseTag = true;
    } else {
        ballUseTag = false;
        ballManualFrame = 0;
        ballManualTimer = 0.0f;
    }
}

void InitApp() {
    // Set up resource directory
    SearchAndSetResourceDir("resources");

    // Load Menu Assets
    wabbit = LoadTexture("wabbit_alpha.png");
    if (FileExists("crystal_cave_track.mp3")) {
        music = LoadMusicStream("crystal_cave_track.mp3");
        PlayMusicStream(music);
        musicLoaded = true;
    }

    InitBallAseprite();

    // Load Shader
    #if defined(PLATFORM_WEB)
        shader = LoadShader(0, "wave_web.fs");
    #else
        shader = LoadShader(0, "wave.fs");
    #endif

    resolutionLoc = GetShaderLocation(shader, "resolution");
    timeLoc = GetShaderLocation(shader, "time");
    pointerLoc = GetShaderLocation(shader, "pointer");
}

void UpdateMenu() {
    if (musicLoaded) {
        UpdateMusicStream(music);
    }

    float deltaTime = GetFrameTime();

    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
    rotation += 90.0f * deltaTime;

    if (position.x <= 0 || position.x >= GetScreenWidth() - wabbit.width) velocity.x *= -1;
    if (position.y <= 0 || position.y >= GetScreenHeight() - wabbit.height) velocity.y *= -1;

    Vector2 mousePoint = GetMousePosition();
    btnShaderState = false;
    btnAsepriteState = false;

    if (CheckCollisionPointRec(mousePoint, buttonBounds)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) btnShaderState = true;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            currentState = STATE_SHADER;
        }
    }

    if (CheckCollisionPointRec(mousePoint, asepriteButtonBounds)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) btnAsepriteState = true;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            currentState = STATE_ASEPRITE;
        }
    }
}

void DrawMenu() {
    ClearBackground(RAYWHITE);

    Rectangle source = { 0, 0, (float)wabbit.width, (float)wabbit.height };
    Rectangle dest = { position.x + wabbit.width/2, position.y + wabbit.height/2, (float)wabbit.width, (float)wabbit.height };
    Vector2 origin = { (float)wabbit.width/2, (float)wabbit.height/2 };
    DrawTexturePro(wabbit, source, dest, origin, rotation, WHITE);

    DrawRectangleRec(buttonBounds, btnShaderState ? LIGHTGRAY : GRAY);
    DrawRectangleLines((int)buttonBounds.x, (int)buttonBounds.y, (int)buttonBounds.width, (int)buttonBounds.height, BLACK);
    DrawText("Interactive Shader 1", (int)buttonBounds.x + 15, (int)buttonBounds.y + 15, 20, BLACK);

    DrawRectangleRec(asepriteButtonBounds, btnAsepriteState ? LIGHTGRAY : GRAY);
    DrawRectangleLines((int)asepriteButtonBounds.x, (int)asepriteButtonBounds.y, (int)asepriteButtonBounds.width, (int)asepriteButtonBounds.height, BLACK);
    DrawText("Aseprite: falling ball", (int)asepriteButtonBounds.x + 10, (int)asepriteButtonBounds.y + 15, 20, BLACK);

    DrawText("Main Menu - quickstart-c-aesprite", 10, 10, 20, DARKGRAY);
}

void UpdateShader() {
    float deltaTime = GetFrameTime();
    shaderTime += deltaTime;

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        shaderTime += wheel * 0.5f;
    }

    float resolution[2] = { (float)GetRenderWidth(), (float)GetRenderHeight() };
    SetShaderValue(shader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, timeLoc, &shaderTime, SHADER_UNIFORM_FLOAT);

    Vector2 mouse = GetMousePosition();
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    float pointerX = (mouse.x - screenWidth / 2.0f) / (screenHeight / 2.0f);
    float pointerY = -(mouse.y - screenHeight / 2.0f) / (screenHeight / 2.0f);
    float pointer[2] = { pointerX, pointerY };
    SetShaderValue(shader, pointerLoc, pointer, SHADER_UNIFORM_VEC2);

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

    DrawRectangleRec(closeButtonBounds, closeBtnState ? RED : MAROON);
    DrawText("X", (int)closeButtonBounds.x + 8, (int)closeButtonBounds.y + 5, 20, WHITE);
    DrawText("Scroll to change speed", 10, GetScreenHeight() - 30, 20, WHITE);
}

void UpdateAsepriteView(void) {
    if (ballUseTag) {
        UpdateAsepriteTag(&ballTag);
    } else {
        ase_t* ase = ballAseprite.ase;
        assert(ase != NULL && ase->frame_count > 0);
        float dt = GetFrameTime();
        int durMs = ase->frames[ballManualFrame].duration_milliseconds;
        if (durMs <= 0) durMs = 100;
        ballManualTimer += dt;
        float durSec = (float)durMs / 1000.0f;
        while (ballManualTimer >= durSec) {
            ballManualTimer -= durSec;
            ballManualFrame = (ballManualFrame + 1) % ase->frame_count;
            durMs = ase->frames[ballManualFrame].duration_milliseconds;
            if (durMs <= 0) durMs = 100;
            durSec = (float)durMs / 1000.0f;
        }
    }

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

void DrawAsepriteView(void) {
    ClearBackground((Color){ 24, 24, 32, 255 });

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float scale = 4.0f;

    if (ballUseTag) {
        float fw = (float)GetAsepriteWidth(ballAseprite);
        float fh = (float)GetAsepriteHeight(ballAseprite);
        Vector2 pos = {
            (float)sw * 0.5f - (fw * scale) * 0.5f,
            (float)sh * 0.5f - (fh * scale) * 0.5f
        };
        DrawAsepriteTagEx(ballTag, pos, 0.0f, scale, WHITE);
    } else {
        float fw = (float)GetAsepriteWidth(ballAseprite);
        float fh = (float)GetAsepriteHeight(ballAseprite);
        Vector2 pos = {
            (float)sw * 0.5f - (fw * scale) * 0.5f,
            (float)sh * 0.5f - (fh * scale) * 0.5f
        };
        DrawAsepriteEx(ballAseprite, ballManualFrame, pos, 0.0f, scale, WHITE);
    }

    DrawRectangleRec(closeButtonBounds, closeBtnState ? RED : MAROON);
    DrawText("X", (int)closeButtonBounds.x + 8, (int)closeButtonBounds.y + 5, 20, WHITE);
    DrawText("Aseprite preview — anim-sprite-ball-falling.aseprite", 10, 10, 18, RAYWHITE);
    DrawText("raylib-aseprite (RobLoach)", 10, sh - 30, 18, LIGHTGRAY);
}

int main(void)
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(1280, 800, "Raylib quickstart-c-aesprite");
    InitAudioDevice();

    InitApp();

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        switch (currentState) {
            case STATE_MENU: UpdateMenu(); break;
            case STATE_SHADER: UpdateShader(); break;
            case STATE_ASEPRITE: UpdateAsepriteView(); break;
        }

        BeginDrawing();
            switch (currentState) {
                case STATE_MENU: DrawMenu(); break;
                case STATE_SHADER: DrawShader(); break;
                case STATE_ASEPRITE: DrawAsepriteView(); break;
            }
        EndDrawing();
    }

    UnloadTexture(wabbit);
    if (musicLoaded) {
        UnloadMusicStream(music);
    }
    UnloadShader(shader);
    UnloadAseprite(ballAseprite);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
