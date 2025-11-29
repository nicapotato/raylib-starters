"""
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/
"""

import os
from pathlib import Path
from pyray import *


def main():
    # Tell the window to use vsync and work on high DPI displays
    set_config_flags(ConfigFlags.FLAG_VSYNC_HINT | ConfigFlags.FLAG_WINDOW_HIGHDPI)

    # Create the window and OpenGL context
    init_window(1280, 800, "Hello Raylib")
    
    # Initialize audio device
    init_audio_device()

    # Set working directory to resources folder if it exists
    # For macOS app bundles, resources are in Contents/Resources/resources/
    # For development, resources are in the project root
    script_dir = Path(__file__).resolve().parent
    # Check macOS app bundle Resources directory
    resources_path = script_dir.parent.parent / "Resources" / "resources"
    if resources_path.is_dir():
        os.chdir(str(resources_path))
    elif (script_dir / "resources").is_dir():
        # Check next to script (development)
        os.chdir(str(script_dir / "resources"))
    elif Path("resources").is_dir():
        # Check current directory (fallback)
        os.chdir("resources")

    # Load a texture from the resources directory
    wabbit = load_texture("wabbit_alpha.png")
    
    # Load music
    music = load_music_stream("crystal_cave_track.mp3")
    play_music_stream(music)
    
    # State variables
    position = Vector2(400.0, 200.0)
    velocity = Vector2(200.0, 200.0)
    rotation = 0.0

    # Game loop
    while not window_should_close():  # Run the loop until the user presses ESCAPE or presses the Close button on the window
        # Update
        update_music_stream(music)
        
        delta_time = get_frame_time()
        
        position.x += velocity.x * delta_time
        position.y += velocity.y * delta_time
        rotation += 90.0 * delta_time
        
        # Bounce logic
        if position.x <= 0 or position.x >= get_screen_width():
            velocity.x *= -1
        if position.y <= 0 or position.y >= get_screen_height():
            velocity.y *= -1

        # Drawing
        begin_drawing()

        # Setup the back buffer for drawing (clear color and depth buffers)
        clear_background(BLACK)

        # Draw some text using the default font
        draw_text("Hello Raylib", 200, 200, 20, WHITE)

        # Draw our texture to the screen
        # Using DrawTexturePro to support rotation
        source = Rectangle(0, 0, float(wabbit.width), float(wabbit.height))
        dest = Rectangle(position.x, position.y, float(wabbit.width), float(wabbit.height))
        origin = Vector2(float(wabbit.width)/2, float(wabbit.height)/2)
        
        draw_texture_pro(wabbit, source, dest, origin, rotation, WHITE)

        # End the frame and get ready for the next one (display frame, poll input, etc...)
        end_drawing()

    # Cleanup
    # Unload our texture so it can be cleaned up
    unload_texture(wabbit)
    unload_music_stream(music)
    
    close_audio_device()

    # Destroy the window and cleanup the OpenGL context
    close_window()


if __name__ == "__main__":
    main()
