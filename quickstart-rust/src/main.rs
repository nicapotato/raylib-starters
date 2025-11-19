/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/
*/

use raylib::prelude::*;
use std::env;
use std::path::PathBuf;

fn main() {
    // Tell the window to use vsync and work on high DPI displays
    let (mut rl, thread) = raylib::init()
        .size(1280, 800)
        .title("Hello Raylib")
        .vsync()
        .build();

    // Set working directory to resources folder if it exists
    // For macOS app bundles, resources are in Contents/Resources/resources/
    // For development builds, resources are copied next to the executable
    if let Ok(exe_path) = env::current_exe() {
        if let Some(exe_dir) = exe_path.parent() {
            // Check macOS app bundle Resources directory
            let mut resources_path = exe_dir.join("..").join("Resources").join("resources");
            resources_path = resources_path.canonicalize().unwrap_or(resources_path);
            if resources_path.is_dir() {
                let _ = env::set_current_dir(&resources_path);
            } else {
                // Check next to executable (development build)
                let resources_path = exe_dir.join("resources");
                if resources_path.is_dir() {
                    let _ = env::set_current_dir(&resources_path);
                } else if PathBuf::from("resources").is_dir() {
                    // Check current directory (fallback)
                    let _ = env::set_current_dir("resources");
                }
            }
        }
    } else if PathBuf::from("resources").is_dir() {
        // Fallback: check current directory
        let _ = env::set_current_dir("resources");
    }

    // Load a texture from the resources directory
    let wabbit = match rl.load_texture(&thread, "wabbit_alpha.png") {
        Ok(texture) => texture,
        Err(e) => {
            eprintln!("Error: Could not load texture 'wabbit_alpha.png': {:?}", e);
            eprintln!("Current working directory: {:?}", env::current_dir().unwrap_or_default());
            eprintln!("Please ensure the resources directory exists and contains wabbit_alpha.png");
            return;
        }
    };

    // Game loop
    while !rl.window_should_close() {
        // Drawing
        let mut d = rl.begin_drawing(&thread);

        // Setup the back buffer for drawing (clear color and depth buffers)
        d.clear_background(Color::BLACK);

        // Draw some text using the default font
        d.draw_text("Hello Raylib", 200, 200, 20, Color::WHITE);

        // Draw our texture to the screen
        d.draw_texture(&wabbit, 400, 200, Color::WHITE);
    }

    // Cleanup happens automatically when variables go out of scope
    // The texture will be unloaded when wabbit is dropped
    // The window will be closed when rl is dropped
}

