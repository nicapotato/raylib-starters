# Raylib-Quickstart-Go

A simple cross-platform template for setting up a raylib project with Go. This is the Go version of the raylib-quickstart template.

## Features

- Basic raylib window with VSync and High DPI support
- Resource directory utility for finding assets
- Example texture loading and drawing
- Cross-platform support (Windows, Linux, macOS)

## Prerequisites

- Go 1.21 or later
- A C compiler (for building raylib bindings)
  - **Windows**: MinGW-W64 or Visual Studio
  - **Linux**: GCC (usually `build-essential` package)
  - **macOS**: Xcode Command Line Tools

## Getting Started

1. **Clone or download this repository**

2. **Install dependencies**
   ```bash
   go mod download
   ```

3. **Run the example**
   ```bash
   go run main.go
   ```

4. **Build the executable**
   ```bash
   go build -o raylib-quickstart-go main.go
   ```

## Project Structure

```
raylib-quickstart-go/
├── main.go          # Main application code
├── go.mod           # Go module definition
├── go.sum           # Go module checksums (generated)
├── README.md        # This file
└── resources/       # Resource files (images, sounds, etc.)
    └── wabbit_alpha.png
```

## How It Works

The `main.go` file demonstrates:

1. **Window Initialization**: Creates a 1280x800 window with VSync and High DPI support
2. **Resource Directory Search**: The `searchAndSetResourceDir()` function searches for the resources folder in common locations:
   - Current working directory
   - Application directory
   - Up to 3 levels above the application directory
3. **Texture Loading**: Loads an image from the resources directory
4. **Game Loop**: Standard raylib game loop with drawing operations
5. **Cleanup**: Properly unloads resources and closes the window

## Customizing

### Changing the Window Size

Edit the `InitWindow` call in `main.go`:
```go
rl.InitWindow(1280, 800, "Hello Raylib")
```

### Adding Your Own Resources

1. Place your resource files in the `resources/` directory
2. Load them using raylib functions:
   ```go
   texture := rl.LoadTexture("your_image.png")
   ```

### Using Your Own Code

Simply replace `main.go` with your own code, or modify the existing file to add your game logic.

## Building for Release

To build an optimized release version:

```bash
go build -ldflags="-s -w" -o raylib-quickstart-go main.go
```

## Troubleshooting

### "Cannot find package" errors

Make sure you've run `go mod download` to fetch dependencies.

### Resource files not found

The `searchAndSetResourceDir()` function will search for the resources folder in several locations. Make sure:
- The `resources/` folder exists in your project directory
- You're running the executable from a location where the resources folder can be found

### C compiler errors

Make sure you have a C compiler installed:
- **Windows**: Install MinGW-W64 or Visual Studio
- **Linux**: `sudo apt-get install build-essential` (Debian/Ubuntu) or equivalent
- **macOS**: `xcode-select --install`

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

