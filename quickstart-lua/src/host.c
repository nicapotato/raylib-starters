/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/
*/

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

Rectangle lua_checkrectangle(lua_State *L, int idx) {
    Rectangle r = {0};
    if (lua_istable(L, idx)) {
        // Support both array-like {x, y, w, h} and field-like {x=.., y=..}
        lua_getfield(L, idx, "x"); 
        if (lua_isnil(L, -1)) { 
             lua_pop(L, 1); lua_rawgeti(L, idx, 1); r.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
             lua_rawgeti(L, idx, 2); r.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
             lua_rawgeti(L, idx, 3); r.width = (float)lua_tonumber(L, -1); lua_pop(L, 1);
             lua_rawgeti(L, idx, 4); r.height = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        } else {
             r.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
             lua_getfield(L, idx, "y"); r.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
             lua_getfield(L, idx, "width"); r.width = (float)lua_tonumber(L, -1); lua_pop(L, 1);
             lua_getfield(L, idx, "height"); r.height = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        }
    }
    return r;
}

Vector2 lua_checkvector2(lua_State *L, int idx) {
    Vector2 v = {0};
    if (lua_istable(L, idx)) {
        lua_getfield(L, idx, "x"); v.x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_getfield(L, idx, "y"); v.y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    }
    return v;
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

static int l_InitAudioDevice(lua_State *L) {
    InitAudioDevice();
    return 0;
}

static int l_CloseAudioDevice(lua_State *L) {
    CloseAudioDevice();
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

static int l_DrawTexturePro(lua_State *L) {
    Texture2D *tex = (Texture2D *)lua_touserdata(L, 1);
    if (!tex) return luaL_error(L, "Expected Texture2D");

    Rectangle source = lua_checkrectangle(L, 2);
    Rectangle dest = lua_checkrectangle(L, 3);
    Vector2 origin = lua_checkvector2(L, 4);
    float rotation = (float)lua_tonumber(L, 5);
    Color tint = lua_checkcolor(L, 6);

    DrawTexturePro(*tex, source, dest, origin, rotation, tint);
    return 0;
}

static int l_GetTextureWidth(lua_State *L) {
    Texture2D *tex = (Texture2D *)lua_touserdata(L, 1);
    if (!tex) return luaL_error(L, "Expected Texture2D");
    lua_pushinteger(L, tex->width);
    return 1;
}

static int l_GetTextureHeight(lua_State *L) {
    Texture2D *tex = (Texture2D *)lua_touserdata(L, 1);
    if (!tex) return luaL_error(L, "Expected Texture2D");
    lua_pushinteger(L, tex->height);
    return 1;
}

static int l_UnloadTexture(lua_State *L) {
    Texture2D *tex = (Texture2D *)lua_touserdata(L, 1);
    if (tex) UnloadTexture(*tex);
    return 0;
}

// Music userdata wrapper
static int l_LoadMusicStream(lua_State *L) {
    const char *fileName = luaL_checkstring(L, 1);
    Music music = LoadMusicStream(fileName);
    Music *ud = (Music *)lua_newuserdata(L, sizeof(Music));
    *ud = music;
    return 1;
}

static int l_PlayMusicStream(lua_State *L) {
    Music *music = (Music *)lua_touserdata(L, 1);
    if (music) PlayMusicStream(*music);
    return 0;
}

static int l_UpdateMusicStream(lua_State *L) {
    Music *music = (Music *)lua_touserdata(L, 1);
    if (music) UpdateMusicStream(*music);
    return 0;
}

static int l_UnloadMusicStream(lua_State *L) {
    Music *music = (Music *)lua_touserdata(L, 1);
    if (music) UnloadMusicStream(*music);
    return 0;
}

static int l_GetFrameTime(lua_State *L) {
    lua_pushnumber(L, GetFrameTime());
    return 1;
}

static int l_GetScreenWidth(lua_State *L) {
    lua_pushinteger(L, GetScreenWidth());
    return 1;
}

static int l_GetScreenHeight(lua_State *L) {
    lua_pushinteger(L, GetScreenHeight());
    return 1;
}

// Helpers for creating structs in Lua
static int l_Rectangle(lua_State *L) {
    lua_createtable(L, 0, 4);
    lua_pushvalue(L, 1); lua_setfield(L, -2, "x");
    lua_pushvalue(L, 2); lua_setfield(L, -2, "y");
    lua_pushvalue(L, 3); lua_setfield(L, -2, "width");
    lua_pushvalue(L, 4); lua_setfield(L, -2, "height");
    return 1;
}

static int l_Vector2(lua_State *L) {
    lua_createtable(L, 0, 2);
    lua_pushvalue(L, 1); lua_setfield(L, -2, "x");
    lua_pushvalue(L, 2); lua_setfield(L, -2, "y");
    return 1;
}

// Colors
static int l_Color_BLACK(lua_State *L) { lua_pushcolor(L, BLACK); return 1; }
static int l_Color_WHITE(lua_State *L) { lua_pushcolor(L, WHITE); return 1; }

static int l_CloseWindow(lua_State *L) {
    CloseWindow();
    return 0;
}

static const struct luaL_Reg raylib_funcs[] = {
    {"SetConfigFlags", l_SetConfigFlags},
    {"InitWindow", l_InitWindow},
    {"InitAudioDevice", l_InitAudioDevice},
    {"CloseAudioDevice", l_CloseAudioDevice},
    {"SetResourceDir", l_SetResourceDir},
    {"WindowShouldClose", l_WindowShouldClose},
    {"BeginDrawing", l_BeginDrawing},
    {"EndDrawing", l_EndDrawing},
    {"ClearBackground", l_ClearBackground},
    {"DrawText", l_DrawText},
    {"LoadTexture", l_LoadTexture},
    {"GetTextureWidth", l_GetTextureWidth},
    {"GetTextureHeight", l_GetTextureHeight},
    {"DrawTexture", l_DrawTexture},
    {"DrawTexturePro", l_DrawTexturePro},
    {"UnloadTexture", l_UnloadTexture},
    {"LoadMusicStream", l_LoadMusicStream},
    {"PlayMusicStream", l_PlayMusicStream},
    {"UpdateMusicStream", l_UpdateMusicStream},
    {"UnloadMusicStream", l_UnloadMusicStream},
    {"GetFrameTime", l_GetFrameTime},
    {"GetScreenWidth", l_GetScreenWidth},
    {"GetScreenHeight", l_GetScreenHeight},
    {"Rectangle", l_Rectangle},
    {"Vector2", l_Vector2},
    {"CloseWindow", l_CloseWindow},
    {NULL, NULL}
};

int luaopen_raylib(lua_State *L) {
    luaL_newlib(L, raylib_funcs);
    
    // Set flags constants
    lua_pushinteger(L, FLAG_VSYNC_HINT); lua_setfield(L, -2, "FLAG_VSYNC_HINT");
    lua_pushinteger(L, FLAG_WINDOW_HIGHDPI); lua_setfield(L, -2, "FLAG_WINDOW_HIGHDPI");
    
    // Set colors as tables
    lua_pushcolor(L, BLACK); lua_setfield(L, -2, "BLACK");
    lua_pushcolor(L, WHITE); lua_setfield(L, -2, "WHITE");
    
    return 1;
}

int main(int argc, char *argv[]) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    
    // Preload raylib module
    luaL_requiref(L, "raylib", luaopen_raylib, 1);
    lua_pop(L, 1);
    
    if (luaL_dofile(L, "src/main.lua") != LUA_OK) {
        fprintf(stderr, "Lua Error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return 1;
    }
    
    lua_close(L);
    return 0;
}
