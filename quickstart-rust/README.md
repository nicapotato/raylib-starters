# Raylib-Quickstart-Rust

A simple cross-platform template for setting up a raylib project with Rust. This is the Rust version of the raylib-quickstart template.

## Features

- Basic raylib window with VSync and High DPI support
- Resource directory utility for finding assets
- Example texture loading and drawing
- Cross-platform support (Windows, Linux, macOS)

## Prerequisites

- Rust 1.70 or later (install from [rustup.rs](https://rustup.rs/))
- A C compiler (for building raylib bindings)
  - **Windows**: MinGW-W64 or Visual Studio Build Tools
  - **Linux**: GCC (usually `build-essential` package)
  - **macOS**: Xcode Command Line Tools

## Getting Started

1. **Clone or download this repository**

2. **Run the example**
   ```bash
   cargo run
   ```

3. **Build the executable**
   ```bash
   cargo build --release
   ```

   The executable will be in `target/release/raylib-quickstart-rust` (or `target/release/raylib-quickstart-rust.exe` on Windows)

## Project Structure

```
raylib-quickstart-rust/
├── src/
│   └── main.rs        # Main application code
├── Cargo.toml         # Rust dependencies and project configuration
├── README.md          # This file
└── resources/         # Resource files (images, sounds, etc.)
    └── wabbit_alpha.png
```

## How It Works

The `src/main.rs` file demonstrates:

1. **Window Initialization**: Creates a 1280x800 window with VSync and High DPI support
2. **Resource Directory Search**: The `search_and_set_resource_dir()` function searches for the resources folder in common locations:
   - Current working directory
   - Application directory (where the executable is located)
   - Up to 3 levels above the application directory
3. **Texture Loading**: Loads an image from the resources directory
4. **Game Loop**: Standard raylib game loop with drawing operations
5. **Cleanup**: Resources are automatically cleaned up when variables go out of scope (Rust's RAII)

## Customizing

### Changing the Window Size

Edit the `raylib::init()` call in `src/main.rs`:
```rust
let (mut rl, thread) = raylib::init()
    .size(1280, 800)
    .title("Hello Raylib")
    .vsync()
    .build();
```

### Adding Your Own Resources

1. Place your resource files in the `resources/` directory
2. Load them using raylib functions:
   ```rust
   let texture = rl.load_texture(&thread, "your_image.png")
       .expect("Could not load texture");
   ```

### Using Your Own Code

Simply replace `src/main.rs` with your own code, or modify the existing file to add your game logic.

## Building for Release

To build an optimized release version:

```bash
cargo build --release
```

The optimized binary will be in `target/release/`.

## Troubleshooting

### "Cannot find raylib" errors

Make sure you have a C compiler installed. The raylib-rs crate needs to compile raylib from source.

### Resource files not found

The `search_and_set_resource_dir()` function will search for the resources folder in several locations. Make sure:
- The `resources/` folder exists in your project directory
- You're running the executable from a location where the resources folder can be found

### C compiler errors

Make sure you have a C compiler installed:
- **Windows**: Install MinGW-W64 or Visual Studio Build Tools
- **Linux**: `sudo apt-get install build-essential` (Debian/Ubuntu) or equivalent
- **macOS**: `xcode-select --install`

### Linker errors on Windows

If you encounter linker errors on Windows, you may need to install the Visual C++ Build Tools or use MinGW-W64. The raylib-rs crate should handle most of this automatically, but some systems may need additional configuration.

## License

This project follows the same license as the original raylib-quickstart template.

Copyright (c) 2020-2025 Jeffery Myers

This software is provided "as-is", without any express or implied warranty. In no event 
will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim that you 
  wrote the original software. If you use this software in a product, an acknowledgment 
  in the product documentation would be appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be misrepresented
  as being the original software.

  3. This notice may not be removed or altered from any source distribution.

