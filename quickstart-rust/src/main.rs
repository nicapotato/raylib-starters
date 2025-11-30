
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
    
    // Initialize audio device
    let mut audio = RaylibAudio::init_audio_device().expect("Could not create audio device");

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
    
    // Load music
    let mut music = match audio.new_music("crystal_cave_track.mp3") {
        Ok(music) => music,
        Err(e) => {
            eprintln!("Error: Could not load music 'crystal_cave_track.mp3': {:?}", e);
            return;
        }
    };
    
    music.play_stream();
    
    // State variables
    let mut position = Vector2::new(400.0, 200.0);
    let mut velocity = Vector2::new(200.0, 200.0);
    let mut rotation = 0.0;

    // Game loop
    while !rl.window_should_close() {
        // Update
        music.update_stream();
        
        let delta_time = rl.get_frame_time();
        
        position.x += velocity.x * delta_time;
        position.y += velocity.y * delta_time;
        rotation += 90.0 * delta_time;
        
        // Bounce logic
        if position.x <= 0.0 || position.x >= rl.get_screen_width() as f32 {
            velocity.x *= -1.0;
        }
        if position.y <= 0.0 || position.y >= rl.get_screen_height() as f32 {
            velocity.y *= -1.0;
        }

        // Drawing
        let mut d = rl.begin_drawing(&thread);

        // Setup the back buffer for drawing (clear color and depth buffers)
        d.clear_background(Color::BLACK);

        // Draw some text using the default font
        d.draw_text("Hello Raylib", 200, 200, 20, Color::WHITE);

        // Draw our texture to the screen
        // Using draw_texture_pro to support rotation
        let source = Rectangle::new(0.0, 0.0, wabbit.width as f32, wabbit.height as f32);
        let dest = Rectangle::new(position.x, position.y, wabbit.width as f32, wabbit.height as f32);
        let origin = Vector2::new(wabbit.width as f32 / 2.0, wabbit.height as f32 / 2.0);
        
        d.draw_texture_pro(&wabbit, source, dest, origin, rotation, Color::WHITE);
    }

    // Cleanup happens automatically when variables go out of scope
}
