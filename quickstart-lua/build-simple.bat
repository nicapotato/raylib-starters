@echo off
setlocal enableextensions enabledelayedexpansion

set "PROJECT_ROOT=%~dp0"
cd /d "%PROJECT_ROOT%"

set "EXTERNAL_DIR=external"
set "RAYLIB_LUA_DIR=%EXTERNAL_DIR%\raylib-lua"
set "RAYLUA_EXEC=%RAYLIB_LUA_DIR%\raylua_s.exe"

if not exist "%EXTERNAL_DIR%" mkdir "%EXTERNAL_DIR%"

echo [1/3] Checking raylib-lua installation...
if not exist "%RAYLIB_LUA_DIR%" (
    echo      raylib-lua not found. Cloning from GitHub...
    git clone --recursive https://github.com/TSnake41/raylib-lua "%RAYLIB_LUA_DIR%"
    if errorlevel 1 (
        echo [ERROR] Failed to clone raylib-lua.
        pause
        exit /b 1
    )
)

echo [2/3] Checking raylib-lua build...
if not exist "%RAYLUA_EXEC%" (
    echo      Building raylib-lua...
    pushd "%RAYLIB_LUA_DIR%"
    git submodule update --init --recursive
    mingw32-make
    if errorlevel 1 (
        popd
        echo [ERROR] Failed to build raylib-lua.
        echo [TIP] You might need to download pre-built binaries from https://github.com/TSnake41/raylib-lua/releases
        pause
        exit /b 1
    )
    popd
)

echo [3/3] Running game...
echo ===================================================
"%RAYLUA_EXEC%" src\main.lua
echo ===================================================
pause

