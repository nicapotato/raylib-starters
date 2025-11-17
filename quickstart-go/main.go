package main

import (
	"os"
	"path/filepath"

	rl "github.com/gen2brain/raylib-go/raylib"
)

func main() {
	// Tell the window to use vsync and work on high DPI displays
	rl.SetConfigFlags(rl.FlagVsyncHint | rl.FlagWindowHighdpi)

	// Create the window and OpenGL context
	rl.InitWindow(1280, 800, "Hello Raylib")

	// Utility function to find the resources folder and set it as the current working directory
	searchAndSetResourceDir("resources")

	// Load a texture from the resources directory
	wabbit := rl.LoadTexture("wabbit_alpha.png")

	// Game loop
	for !rl.WindowShouldClose() { // Run the loop until the user presses ESCAPE or presses the Close button on the window
		// Drawing
		rl.BeginDrawing()

		// Setup the back buffer for drawing (clear color and depth buffers)
		rl.ClearBackground(rl.Black)

		// Draw some text using the default font
		rl.DrawText("Hello Raylib", 200, 200, 20, rl.White)

		// Draw our texture to the screen
		rl.DrawTexture(wabbit, 400, 200, rl.White)

		// End the frame and get ready for the next one (display frame, poll input, etc...)
		rl.EndDrawing()
	}

	// Cleanup
	// Unload our texture so it can be cleaned up
	rl.UnloadTexture(wabbit)

	// Destroy the window and cleanup the OpenGL context
	rl.CloseWindow()
}

// searchAndSetResourceDir looks for the specified resource dir in several common locations:
// - The working dir
// - The app dir
// - Up to 3 levels above the app dir
// When found the dir will be set as the working dir so that assets can be loaded relative to that.
func searchAndSetResourceDir(folderName string) bool {
	// Check the working dir
	if dirExists(folderName) {
		os.Chdir(folderName)
		return true
	}

	// Get the application directory
	appDir, err := os.Executable()
	if err != nil {
		return false
	}
	appDir = filepath.Dir(appDir)

	// Check the application dir
	dir := filepath.Join(appDir, folderName)
	if dirExists(dir) {
		os.Chdir(dir)
		return true
	}

	// Check one up from the app dir
	dir = filepath.Join(appDir, "..", folderName)
	if dirExists(dir) {
		os.Chdir(dir)
		return true
	}

	// Check two up from the app dir
	dir = filepath.Join(appDir, "..", "..", folderName)
	if dirExists(dir) {
		os.Chdir(dir)
		return true
	}

	// Check three up from the app dir
	dir = filepath.Join(appDir, "..", "..", "..", folderName)
	if dirExists(dir) {
		os.Chdir(dir)
		return true
	}

	return false
}

// dirExists checks if a directory exists
func dirExists(path string) bool {
	info, err := os.Stat(path)
	if err != nil {
		return false
	}
	return info.IsDir()
}

