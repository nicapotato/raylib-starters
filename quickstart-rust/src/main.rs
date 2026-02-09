use raylib::ffi;
use raylib::prelude::*;
use std::env;
use std::ffi::CString;
#[cfg(not(target_os = "emscripten"))]
use std::path::PathBuf;

// --- Emscripten interop ---
#[cfg(target_os = "emscripten")]
extern "C" {
    fn emscripten_set_main_loop(func: extern "C" fn(), fps: i32, simulate_infinite_loop: i32);
}

// --- Game state ---
struct GameState {
    rl: RaylibHandle,
    thread: RaylibThread,
    wabbit: Texture2D,
    music: Option<ffi::Music>,
    position: Vector2,
    velocity: Vector2,
    rotation: f32,
}

// WASM is single-threaded, so this is safe.
#[cfg(target_os = "emscripten")]
struct UnsafeSendSync<T>(T);
#[cfg(target_os = "emscripten")]
unsafe impl<T> Send for UnsafeSendSync<T> {}
#[cfg(target_os = "emscripten")]
unsafe impl<T> Sync for UnsafeSendSync<T> {}

#[cfg(target_os = "emscripten")]
static mut GAME: Option<UnsafeSendSync<GameState>> = None;

fn update_and_draw(state: &mut GameState) {
    // Update music stream
    if let Some(music) = state.music {
        unsafe { ffi::UpdateMusicStream(music); }
    }

    let delta_time = state.rl.get_frame_time();

    state.position.x += state.velocity.x * delta_time;
    state.position.y += state.velocity.y * delta_time;
    state.rotation += 90.0 * delta_time;

    // Bounce logic
    if state.position.x <= 0.0 || state.position.x >= state.rl.get_screen_width() as f32 {
        state.velocity.x *= -1.0;
    }
    if state.position.y <= 0.0 || state.position.y >= state.rl.get_screen_height() as f32 {
        state.velocity.y *= -1.0;
    }

    // Drawing
    let mut d = state.rl.begin_drawing(&state.thread);
    d.clear_background(Color::BLACK);
    d.draw_text("Hello Raylib", 200, 200, 20, Color::WHITE);

    let source = Rectangle::new(0.0, 0.0, state.wabbit.width as f32, state.wabbit.height as f32);
    let dest = Rectangle::new(
        state.position.x,
        state.position.y,
        state.wabbit.width as f32,
        state.wabbit.height as f32,
    );
    let origin = Vector2::new(
        state.wabbit.width as f32 / 2.0,
        state.wabbit.height as f32 / 2.0,
    );
    d.draw_texture_pro(&state.wabbit, source, dest, origin, state.rotation, Color::WHITE);
}

/// Callback invoked by emscripten each frame.
#[cfg(target_os = "emscripten")]
extern "C" fn emscripten_loop() {
    unsafe {
        if let Some(ref mut wrapper) = GAME {
            update_and_draw(&mut wrapper.0);
        }
    }
}

fn setup_resource_path() {
    #[cfg(target_os = "emscripten")]
    {
        // Emscripten virtual filesystem has resources at /resources
        // (via --preload-file resources@resources)
        let _ = env::set_current_dir("resources");
        return;
    }

    #[cfg(not(target_os = "emscripten"))]
    {
        if let Ok(exe_path) = env::current_exe() {
            if let Some(exe_dir) = exe_path.parent() {
                // Check macOS app bundle Resources directory
                let mut resources_path = exe_dir.join("..").join("Resources").join("resources");
                resources_path = resources_path.canonicalize().unwrap_or(resources_path);
                if resources_path.is_dir() {
                    let _ = env::set_current_dir(&resources_path);
                    return;
                }
                // Check next to executable (development build)
                let resources_path = exe_dir.join("resources");
                if resources_path.is_dir() {
                    let _ = env::set_current_dir(&resources_path);
                    return;
                }
            }
        }
        if PathBuf::from("resources").is_dir() {
            let _ = env::set_current_dir("resources");
        }
    }
}

fn main() {
    let (mut rl, thread) = raylib::init()
        .size(1280, 800)
        .title("Hello Raylib")
        .vsync()
        .build();

    setup_resource_path();

    // Initialize audio using raw FFI (avoids Music<'a> lifetime issue for WASM)
    unsafe { ffi::InitAudioDevice(); }

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

    // Load and play music using raw FFI
    let music_path = CString::new("crystal_cave_track.mp3").unwrap();
    let music = unsafe { ffi::LoadMusicStream(music_path.as_ptr()) };
    let music = if unsafe { ffi::IsMusicValid(music) } {
        unsafe { ffi::PlayMusicStream(music); }
        Some(music)
    } else {
        eprintln!("Warning: Could not load music 'crystal_cave_track.mp3', continuing without audio");
        None
    };

    let state = GameState {
        rl,
        thread,
        wabbit,
        music,
        position: Vector2::new(400.0, 200.0),
        velocity: Vector2::new(200.0, 200.0),
        rotation: 0.0,
    };

    // --- WASM: hand control to emscripten's main loop ---
    #[cfg(target_os = "emscripten")]
    unsafe {
        GAME = Some(UnsafeSendSync(state));
        emscripten_set_main_loop(emscripten_loop, 0, 1);
    }

    // --- Native: regular game loop ---
    #[cfg(not(target_os = "emscripten"))]
    {
        let mut state = state;
        while !state.rl.window_should_close() {
            update_and_draw(&mut state);
        }
        // Clean up audio
        if let Some(music) = state.music {
            unsafe {
                ffi::UnloadMusicStream(music);
                ffi::CloseAudioDevice();
            }
        }
    }
}
