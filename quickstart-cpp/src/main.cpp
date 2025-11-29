/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 800, "Hello Raylib");
	
	// Initialize audio device
	InitAudioDevice();

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");
	
	// Load music
	Music music = LoadMusicStream("crystal_cave_track.mp3");
	PlayMusicStream(music);
	
	// State variables
	Vector2 position = { 400.0f, 200.0f };
	Vector2 velocity = { 200.0f, 200.0f };
	float rotation = 0.0f;
	
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		// Update
		UpdateMusicStream(music);
		
		float deltaTime = GetFrameTime();
		
		position.x += velocity.x * deltaTime;
		position.y += velocity.y * deltaTime;
		rotation += 90.0f * deltaTime;
		
		// Bounce logic
		if (position.x <= 0 || position.x >= GetScreenWidth() - wabbit.width) velocity.x *= -1;
		if (position.y <= 0 || position.y >= GetScreenHeight() - wabbit.height) velocity.y *= -1;

		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// draw some text using the default font
		DrawText("Hello Raylib", 200,200,20,WHITE);

		// draw our texture to the screen
		// Using DrawTexturePro to support rotation
		Rectangle source = { 0, 0, (float)wabbit.width, (float)wabbit.height };
		Rectangle dest = { position.x + wabbit.width/2, position.y + wabbit.height/2, (float)wabbit.width, (float)wabbit.height };
		Vector2 origin = { (float)wabbit.width/2, (float)wabbit.height/2 };
		
		DrawTexturePro(wabbit, source, dest, origin, rotation, WHITE);
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);
	UnloadMusicStream(music);
	
	CloseAudioDevice();

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
