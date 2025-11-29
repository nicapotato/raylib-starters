@echo off
setlocal enableextensions enabledelayedexpansion

set "PROJECT_ROOT=%~dp0"
cd /d "%PROJECT_ROOT%"

set "RAYLIB_DIR=raylib"
set "RAYLIB_SRC=%RAYLIB_DIR%\src"
set "BIN_DIR=bin"
set "OBJ_DIR=obj"

if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"

echo [1/4] Checking Raylib installation...
if not exist "%RAYLIB_DIR%" (
    echo      Raylib not found. Cloning from GitHub...
    git clone --depth 1 https://github.com/raysan5/raylib.git "%RAYLIB_DIR%"
    if errorlevel 1 (
        echo [ERROR] Failed to clone raylib.
        pause
        exit /b 1
    )
)

echo [2/4] Compiling resources...
set "RSRC_OBJ="
where windres >nul 2>nul
if %errorlevel% equ 0 (
    windres src/application.rc -o "%OBJ_DIR%\resources.o" -I src
    if exist "%OBJ_DIR%\resources.o" set "RSRC_OBJ=%OBJ_DIR%\resources.o"
) else (
    echo      windres not found. Skipping resources.
)

echo [3/4] Compiling game and Raylib with Zig cc...
if exist "%BIN_DIR%\game.exe" del "%BIN_DIR%\game.exe"

rem Compile everything from source to avoid ABI issues with pre-built libs.
rem We use -DPLATFORM_DESKTOP to tell Raylib we are building for desktop.
rem We link against system libraries required by Raylib/GLFW on Windows.
zig cc src/main.c "%RAYLIB_SRC%/rcore.c" "%RAYLIB_SRC%/rmodels.c" "%RAYLIB_SRC%/rshapes.c" "%RAYLIB_SRC%/rtext.c" "%RAYLIB_SRC%/rtextures.c" "%RAYLIB_SRC%/utils.c" "%RAYLIB_SRC%/raudio.c" "%RAYLIB_SRC%/rglfw.c" %RSRC_OBJ% -o "%BIN_DIR%\game.exe" -O2 -I include -I "%RAYLIB_SRC%" -I "%RAYLIB_SRC%/external/glfw/include" -DPLATFORM_DESKTOP -lgdi32 -lwinmm -luser32 -lshell32 -lkernel32
if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo [4/4] Running game...
echo ===================================================
cd "%BIN_DIR%"
if not exist "resources" mkdir "resources"
xcopy /s /e /y "..\resources" "resources" >nul
copy /y "..\..\resources\crystal_cave_track.mp3" "resources\" >nul
if exist game.exe game.exe
cd ..
echo ===================================================
echo Execution finished.
pause
