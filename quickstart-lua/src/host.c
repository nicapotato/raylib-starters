#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "resource_dir.h"

// Lua includes
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// Compatibility for Lua 5.1/LuaJIT
#if LUA_VERSION_NUM < 502
    #define luaL_newlib(L,f) (lua_newtable(L), luaL_register(L,NULL,f))
#endif

// --- Helpers ---

Color lua_checkcolor(lua_State *L, int idx) {
    Color c = {0, 0, 0, 255};
    if (lua_istable(L, idx)) {
        lua_getfield(L, idx, "r"); c.r = (unsigned char)lua_tointeger(L, -1); lua_pop(L, 1);
        lua_getfield(L, idx, "g"); c.g = (unsigned char)lua_tointeger(L, -1); lua_pop(L, 1);
        lua_getfield(L, idx, "b"); c.b = (unsigned char)lua_tointeger(L, -1); lua_pop(L, 1);
        lua_getfield(L, idx, "a"); 
        if (!lua_isnil(L, -1)) c.a = (unsigned char)lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
    return c;
}

void lua_pushcolor(lua_State *L, Color c) {
    lua_createtable(L, 0, 4);
    lua_pushinteger(L, c.r); lua_setfield(L, -2, "r");
    lua_pushinteger(L, c.g); lua_setfield(L, -2, "g");
    lua_pushinteger(L, c.b); lua_setfield(L, -2, "b");
    lua_pushinteger(L, c.a); lua_setfield(L, -2, "a");
}

// --- Bindings ---

static int l_SetConfigFlags(lua_State *L) {
    unsigned int flags = (unsigned int)luaL_checkinteger(L, 1);
    SetConfigFlags(flags);
    return 0;
}

static int l_InitWindow(lua_State *L) {
    int width = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);
    const char *title = luaL_checkstring(L, 3);
    InitWindow(width, height, title);
    return 0;
}

static int l_SetResourceDir(lua_State *L) {
    const char *dir = luaL_checkstring(L, 1);
    bool result = SearchAndSetResourceDir(dir);
    lua_pushboolean(L, result);
    return 1;
}

static int l_WindowShouldClose(lua_State *L) {
    lua_pushboolean(L, WindowShouldClose());
    return 1;
}

static int l_BeginDrawing(lua_State *L) {
    BeginDrawing();
    return 0;
}

static int l_EndDrawing(lua_State *L) {
    EndDrawing();
    return 0;
}

static int l_ClearBackground(lua_State *L) {
    Color c = lua_checkcolor(L, 1);
    ClearBackground(c);
    return 0;
}

static int l_DrawText(lua_State *L) {
    const char *text = luaL_checkstring(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int fontSize = luaL_checkinteger(L, 4);
    Color color = lua_checkcolor(L, 5);
    DrawText(text, x, y, fontSize, color);
    return 0;
}

// Texture2D userdata wrapper
static int l_LoadTexture(lua_State *L) {
    const char *fileName = luaL_checkstring(L, 1);
    Texture2D tex = LoadTexture(fileName);
    Texture2D *ud = (Texture2D *)lua_newuserdata(L, sizeof(Texture2D));
    *ud = tex;
    return 1;
}

static int l_DrawTexture(lua_State *L) {
    Texture2D *tex = (Texture2D *)lua_touserdata(L, 1);
    if (!tex) return luaL_error(L, "Expected Texture2D");
    
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    Color tint = lua_checkcolor(L, 4);
    DrawTexture(*tex, x, y, tint);
    return 0;
}

static int l_UnloadTexture(lua_State *L) {
    Texture2D *tex = (Texture2D *)lua_touserdata(L, 1);
    if (tex) UnloadTexture(*tex);
    return 0;
}

static int l_CloseWindow(lua_State *L) {
    CloseWindow();
    return 0;
}

static const struct luaL_Reg raylib_funcs[] = {
    {"SetConfigFlags", l_SetConfigFlags},
    {"InitWindow", l_InitWindow},
    {"SetResourceDir", l_SetResourceDir},
    {"WindowShouldClose", l_WindowShouldClose},
    {"BeginDrawing", l_BeginDrawing},
    {"EndDrawing", l_EndDrawing},
    {"ClearBackground", l_ClearBackground},
    {"DrawText", l_DrawText},
    {"LoadTexture", l_LoadTexture},
    {"DrawTexture", l_DrawTexture},
    {"UnloadTexture", l_UnloadTexture},
    {"CloseWindow", l_CloseWindow},
    {NULL, NULL}
};

int luaopen_raylib(lua_State *L) {
    luaL_newlib(L, raylib_funcs);
    
    // Constants
    lua_pushinteger(L, FLAG_VSYNC_HINT); lua_setfield(L, -2, "FLAG_VSYNC_HINT");
    lua_pushinteger(L, FLAG_WINDOW_HIGHDPI); lua_setfield(L, -2, "FLAG_WINDOW_HIGHDPI");
    
    lua_pushcolor(L, WHITE); lua_setfield(L, -2, "WHITE");
    lua_pushcolor(L, BLACK); lua_setfield(L, -2, "BLACK");
    
    return 1;
}

int main(int argc, char **argv) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // Preload 'raylib' module
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    lua_pushcfunction(L, luaopen_raylib);
    lua_setfield(L, -2, "raylib");
    lua_pop(L, 2);

    if (luaL_dofile(L, "src/main.lua")) {
        fprintf(stderr, "Lua Error: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return 1;
    }

    lua_close(L);
    return 0;
}

