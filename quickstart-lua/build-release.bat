@echo off
setlocal enableextensions enabledelayedexpansion

set "PROJECT_ROOT=%~dp0"
cd /d "%PROJECT_ROOT%"

set "EXTERNAL_DIR=external"
set "RAYLIB_LUA_DIR=%EXTERNAL_DIR%\raylib-lua"
set "RAYLUA_EXEC=%RAYLIB_LUA_DIR%\raylua_s.exe"
set "RELEASE_DIR=release"

if not exist "%EXTERNAL_DIR%" mkdir "%EXTERNAL_DIR%"
if not exist "%RELEASE_DIR%" mkdir "%RELEASE_DIR%"

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
        pause
        exit /b 1
    )
    popd
)

echo [3/3] Packaging game...
echo      Copying executable...
copy /Y "%RAYLUA_EXEC%" "%RELEASE_DIR%\game.exe" >nul

echo      Copying scripts...
copy /Y "src\main.lua" "%RELEASE_DIR%\main.lua" >nul

echo      Copying resources...
if not exist "%RELEASE_DIR%\resources" mkdir "%RELEASE_DIR%\resources"
xcopy /s /e /y "resources" "%RELEASE_DIR%\resources" >nul

echo      Creating launcher...
echo @echo off > "%RELEASE_DIR%\play.bat"
echo start "" game.exe main.lua >> "%RELEASE_DIR%\play.bat"

echo ===================================================
echo Build Success!
echo.
echo Your shareable game is in the "%RELEASE_DIR%" folder.
echo To play, run "play.bat" inside the release folder.
echo ===================================================
pause

