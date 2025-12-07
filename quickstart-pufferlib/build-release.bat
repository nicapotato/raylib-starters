@echo off
setlocal enableextensions enabledelayedexpansion

set "PROJECT_ROOT=%~dp0"
cd /d "%PROJECT_ROOT%"

set "RAYLIB_DIR=raylib"
set "RAYLIB_SRC=%RAYLIB_DIR%\src"
set "BIN_DIR=bin"
set "OBJ_DIR=obj"
set "RELEASE_DIR=release"

if not exist "%RELEASE_DIR%" mkdir "%RELEASE_DIR%"
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

echo [2/4] Checking Raylib build...
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

echo [3/4] Compiling resources...
windres src/application.rc -o "%OBJ_DIR%\resources.o" -I src
if errorlevel 1 (
    echo [WARNING] Failed to compile resources. Icon might be missing.
)

set "RSRC_OBJ="
if exist "%OBJ_DIR%\resources.o" set "RSRC_OBJ=%OBJ_DIR%\resources.o"

echo [4/4] Building standalone game (Static)...
if exist "%RELEASE_DIR%\game.exe" del "%RELEASE_DIR%\game.exe"

:: -static prevents dependency on MinGW DLLs
:: -mwindows (optional) would hide the console window, but we'll leave it for now so you can see printf output
gcc src/main.c %RSRC_OBJ% -o "%RELEASE_DIR%\game.exe" -O2 -I include -I "%RAYLIB_SRC%" -L "%RAYLIB_SRC%" -lraylib -lgdi32 -lwinmm -static

if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo.
echo Packaging resources...
if exist "resources" (
    if not exist "%RELEASE_DIR%\resources" mkdir "%RELEASE_DIR%\resources"
    xcopy /s /e /y "resources" "%RELEASE_DIR%\resources" >nul
    copy /y "..\resources\crystal_cave_track.mp3" "%RELEASE_DIR%\resources\" >nul
)

echo.
echo [5/5] Creating Distribution ZIP...
set "ZIP_NAME=quickstart-pufferlib-win-x64.zip"
if exist "%RELEASE_DIR%\%ZIP_NAME%" del "%RELEASE_DIR%\%ZIP_NAME%"
powershell Compress-Archive -Path "%RELEASE_DIR%\game.exe", "%RELEASE_DIR%\resources" -DestinationPath "%RELEASE_DIR%\%ZIP_NAME%" -Force

rem Read version from project.conf
set "VERSION=latest"
if exist "project.conf" (
    for /f "tokens=1,2 delims==" %%a in (project.conf) do (
        if "%%a"=="VERSION" set "VERSION=%%b"
    )
)

echo.
echo [6/5] Uploading to S3 (Version: %VERSION%)...
echo Uploading to s3://dev-nicapotato-user-content/games/quickstart-c/%VERSION%/windows/
aws s3 cp "%RELEASE_DIR%\%ZIP_NAME%" "s3://dev-nicapotato-user-content/games/quickstart-c/%VERSION%/windows/"
if errorlevel 1 (
    echo [WARNING] S3 upload failed. Make sure AWS CLI is installed and configured.
) else (
    echo Upload complete.
)

echo ===================================================
echo Build Success!
echo.
echo Distribution package created: "%RELEASE_DIR%\%ZIP_NAME%"
echo.
echo Instructions for users:
echo 1. Download and Unzip.
echo 2. Run game.exe.
echo ===================================================
pause
