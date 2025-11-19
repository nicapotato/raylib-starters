/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/
*/

use raylib::prelude::*;
use std::env;
use std::path::PathBuf;

/// Looks for the specified resource dir in several common locations:
/// - The working dir
/// - The app dir
/// - Up to 3 levels above the app dir
/// When found the dir will be set as the working dir so that assets can be loaded relative to that.
fn search_and_set_resource_dir(folder_name: &str) -> bool {
    // Check the working dir
    if PathBuf::from(folder_name).is_dir() {
        env::set_current_dir(folder_name).ok();
        return true;
    }

    // Get the application directory
    let app_dir = match env::current_exe() {
        Ok(exe_path) => match exe_path.parent() {
            Some(parent) => parent.to_path_buf(),
            None => return false,
        },
        Err(_) => return false,
    };

    // Check the application dir
    let dir_path = app_dir.join(folder_name);
    if dir_path.is_dir() {
        env::set_current_dir(&dir_path).ok();
        return true;
    }

    // Check one up from the app dir
    if let Some(parent) = app_dir.parent() {
        let dir_path = parent.join(folder_name);
        if dir_path.is_dir() {
            env::set_current_dir(&dir_path).ok();
            return true;
        }
    }

    // Check two up from the app dir
    if let Some(parent) = app_dir.parent() {
        if let Some(grandparent) = parent.parent() {
            let dir_path = grandparent.join(folder_name);
            if dir_path.is_dir() {
                env::set_current_dir(&dir_path).ok();
                return true;
            }
        }
    }

    // Check three up from the app dir
    if let Some(parent) = app_dir.parent() {
        if let Some(grandparent) = parent.parent() {
            if let Some(great_grandparent) = grandparent.parent() {
                let dir_path = great_grandparent.join(folder_name);
                if dir_path.is_dir() {
                    env::set_current_dir(&dir_path).ok();
                    return true;
                }
            }
        }
    }

    false
}

fn main() {
    // Tell the window to use vsync and work on high DPI displays
    let (mut rl, thread) = raylib::init()
        .size(1280, 800)
        .title("Hello Raylib")
        .vsync()
        .build();

    // Utility function to find the resources folder and set it as the current working directory
    if !search_and_set_resource_dir("resources") {
        eprintln!("Warning: Could not find resources directory. Texture loading may fail.");
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

