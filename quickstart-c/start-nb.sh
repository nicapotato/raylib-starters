#!/bin/bash

# Navigate to project directory
cd /Users/nicapotato/Documents/repo/reading/C/ray-stuff/raylib-quickstart

# Configuration (change as needed: debug_x64, release_x64, debug_arm64, release_arm64, etc.)
CONFIG=debug_x64

# Determine build config (Debug or Release) from CONFIG
if [[ $CONFIG == *"debug"* ]]; then
    BUILD_CONFIG="Debug"
else
    BUILD_CONFIG="Release"
fi

# CLEAN
echo "==== Cleaning build ===="
make clean

# BUILD
echo "==== Building project ===="
make config=$CONFIG

# RUN
echo "==== Running application ===="
./bin/$BUILD_CONFIG/raylib-quickstart

# BUILD DMG (macOS only)
echo "==== Creating DMG package ===="
make config=$CONFIG dmg

# MAC SETUP FIRST TIME LINK RAYLIB
cd /Users/nicapotato/Documents/repo/reading/C/ray-stuff/raylib-quickstart/build
./premake5.osx gmake