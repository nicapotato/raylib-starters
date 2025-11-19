"""
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/
"""

import os
from pathlib import Path
from pyray import *


def search_and_set_resource_dir(folder_name: str) -> bool:
    """
    Looks for the specified resource dir in several common locations:
    - The working dir
    - The app dir
    - Up to 3 levels above the app dir
    When found the dir will be set as the working dir so that assets can be loaded relative to that.
    """
    # Check the working dir
    if os.path.isdir(folder_name):
        os.chdir(folder_name)
        return True

    # Get the application directory
    app_dir = Path(__file__).resolve().parent

    # Check the application dir
    dir_path = app_dir / folder_name
    if dir_path.is_dir():
        os.chdir(str(dir_path))
        return True

    # Check one up from the app dir
    dir_path = app_dir.parent / folder_name
    if dir_path.is_dir():
        os.chdir(str(dir_path))
        return True

    # Check two up from the app dir
    dir_path = app_dir.parent.parent / folder_name
    if dir_path.is_dir():
        os.chdir(str(dir_path))
        return True

    # Check three up from the app dir
    dir_path = app_dir.parent.parent.parent / folder_name
    if dir_path.is_dir():
        os.chdir(str(dir_path))
        return True

    return False


def main():
    # Tell the window to use vsync and work on high DPI displays
    set_config_flags(ConfigFlags.FLAG_VSYNC_HINT | ConfigFlags.FLAG_WINDOW_HIGHDPI)

    # Create the window and OpenGL context
    init_window(1280, 800, "Hello Raylib")

    # Utility function to find the resources folder and set it as the current working directory
    search_and_set_resource_dir("resources")

    # Load a texture from the resources directory
    wabbit = load_texture("wabbit_alpha.png")

    # Game loop
    while not window_should_close():  # Run the loop until the user presses ESCAPE or presses the Close button on the window
        # Drawing
        begin_drawing()

        # Setup the back buffer for drawing (clear color and depth buffers)
        clear_background(BLACK)

        # Draw some text using the default font
        draw_text("Hello Raylib", 200, 200, 20, WHITE)

        # Draw our texture to the screen
        draw_texture(wabbit, 400, 200, WHITE)

        # End the frame and get ready for the next one (display frame, poll input, etc...)
        end_drawing()

    # Cleanup
    # Unload our texture so it can be cleaned up
    unload_texture(wabbit)

    # Destroy the window and cleanup the OpenGL context
    close_window()


if __name__ == "__main__":
    main()

