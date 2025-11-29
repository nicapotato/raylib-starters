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

	// Initialize audio device
	rl.InitAudioDevice()

	// Set working directory to resources folder if it exists
	// For macOS app bundles, resources are in Contents/Resources/resources/
	// For development builds, resources are copied next to the executable
	if exePath, err := os.Executable(); err == nil {
		exeDir := filepath.Dir(exePath)
		// Check macOS app bundle Resources directory
		resourcesPath := filepath.Join(exeDir, "..", "Resources", "resources")
		if absPath, err := filepath.Abs(resourcesPath); err == nil && dirExists(absPath) {
			os.Chdir(absPath)
		} else if resourcesPath := filepath.Join(exeDir, "resources"); dirExists(resourcesPath) {
			// Check next to executable (development build)
			os.Chdir(resourcesPath)
		} else if dirExists("resources") {
			// Check current directory (fallback)
			os.Chdir("resources")
		}
	} else if dirExists("resources") {
		// Fallback: check current directory
		os.Chdir("resources")
	}

	// Load a texture from the resources directory
	wabbit := rl.LoadTexture("wabbit_alpha.png")

	// Load music
	music := rl.LoadMusicStream("crystal_cave_track.mp3")
	rl.PlayMusicStream(music)

	// State variables
	position := rl.NewVector2(400.0, 200.0)
	velocity := rl.NewVector2(200.0, 200.0)
	rotation := float32(0.0)

	// Game loop
	for !rl.WindowShouldClose() { // Run the loop until the user presses ESCAPE or presses the Close button on the window
		// Update
		rl.UpdateMusicStream(music)

		deltaTime := rl.GetFrameTime()

		position.X += velocity.X * deltaTime
		position.Y += velocity.Y * deltaTime
		rotation += 90.0 * deltaTime

		// Bounce logic
		if position.X <= 0 || position.X >= float32(rl.GetScreenWidth()) {
			velocity.X *= -1
		}
		if position.Y <= 0 || position.Y >= float32(rl.GetScreenHeight()) {
			velocity.Y *= -1
		}

		// Drawing
		rl.BeginDrawing()

		// Setup the back buffer for drawing (clear color and depth buffers)
		rl.ClearBackground(rl.Black)

		// Draw some text using the default font
		rl.DrawText("Hello Raylib", 200, 200, 20, rl.White)

		// Draw our texture to the screen
		// Using DrawTexturePro to support rotation
		source := rl.NewRectangle(0, 0, float32(wabbit.Width), float32(wabbit.Height))
		dest := rl.NewRectangle(position.X, position.Y, float32(wabbit.Width), float32(wabbit.Height))
		origin := rl.NewVector2(float32(wabbit.Width)/2, float32(wabbit.Height)/2)

		rl.DrawTexturePro(wabbit, source, dest, origin, rotation, rl.White)

		// End the frame and get ready for the next one (display frame, poll input, etc...)
		rl.EndDrawing()
	}

	// Cleanup
	// Unload our texture so it can be cleaned up
	rl.UnloadTexture(wabbit)
	rl.UnloadMusicStream(music)

	rl.CloseAudioDevice()

	// Destroy the window and cleanup the OpenGL context
	rl.CloseWindow()
}

// dirExists checks if a directory exists
func dirExists(path string) bool {
	info, err := os.Stat(path)
	if err != nil {
		return false
	}
	return info.IsDir()
}
