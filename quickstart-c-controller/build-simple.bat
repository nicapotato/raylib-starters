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

echo [1/5] Checking Raylib installation...
if not exist "%RAYLIB_DIR%" (
    echo      Raylib not found. Cloning from GitHub...
    git clone --depth 1 https://github.com/raysan5/raylib.git "%RAYLIB_DIR%"
    if errorlevel 1 (
        echo [ERROR] Failed to clone raylib.
        pause
        exit /b 1
    )
)

echo [2/5] Checking Raylib build...
if not exist "%RAYLIB_SRC%\libraylib.a" (
    echo      Building Raylib library...
    pushd "%RAYLIB_SRC%"
    mingw32-make -j4
    if errorlevel 1 (
        popd
        echo [ERROR] Failed to build raylib.
        pause
        exit /b 1
    )
    popd
)

echo [3/5] Compiling resources...
rem Include src directory so it finds icon.ico
windres src/application.rc -o "%OBJ_DIR%\resources.o" -I src
if errorlevel 1 (
    echo [WARNING] Failed to compile resources. Icon might be missing.
    rem Create empty resource object if failed to prevent link error if we keep it in gcc command? 
    rem No, we'll just continue, GCC might fail if object doesn't exist.
    rem Better to skip resource linking if windres fails, but let's hope it works.
)

echo [4/5] Compiling game...
rem Clean old exe
if exist "%BIN_DIR%\game.exe" del "%BIN_DIR%\game.exe"

rem Check if resource object exists before linking
set "RSRC_OBJ="
if exist "%OBJ_DIR%\resources.o" set "RSRC_OBJ=%OBJ_DIR%\resources.o"

gcc src/main.c %RSRC_OBJ% -o "%BIN_DIR%\game.exe" -O2 -I include -I "%RAYLIB_SRC%" -L "%RAYLIB_SRC%" -lraylib -lgdi32 -lwinmm
if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo [5/5] Running game...
echo ===================================================
cd "%BIN_DIR%"
if not exist "resources" mkdir "resources"
xcopy /s /e /y "..\resources" "resources" >nul
copy /y "..\..\resources\crystal_cave_track.mp3" "resources\" >nul
game.exe
cd ..
echo ===================================================
echo Execution finished.
pause




