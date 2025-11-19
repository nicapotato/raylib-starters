# Raylib-Quickstart-Python

A simple cross-platform template for setting up a raylib project with Python. This is the Python version of the raylib-quickstart template.

## Features

- Basic raylib window with VSync and High DPI support
- Resource directory utility for finding assets
- Example texture loading and drawing
- Cross-platform support (Windows, Linux, macOS)

## Prerequisites

- Python 3.8 or later
- [uv](https://github.com/astral-sh/uv) (Python package manager)

### Installing uv

Install uv using one of the following methods:

**macOS/Linux:**
```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
```

**Windows:**
```powershell
powershell -ExecutionPolicy ByPass -c "irm https://astral.sh/uv/install.ps1 | iex"
```

Or visit [https://github.com/astral-sh/uv](https://github.com/astral-sh/uv) for more installation options.

## Getting Started

1. **Clone or download this repository**

2. **Install dependencies and run the example**
   ```bash
   make install
   make run
   ```

   Or manually:
   ```bash
   uv sync --no-install-project
   uv run python main.py
   ```

## Project Structure

```
raylib-quickstart-python/
├── main.py          # Main application code
├── Makefile         # Build and run targets
├── pyproject.toml   # Python project configuration and dependencies
├── requirements.txt # Python dependencies (legacy, for reference)
├── README.md        # This file
└── resources/       # Resource files (images, sounds, etc.)
    └── wabbit_alpha.png
```

## Makefile Targets

The project includes a Makefile with convenient targets:

- `make install` - Install dependencies using uv
- `make run` - Install dependencies and run the application
- `make clean` - Remove virtual environment and build artifacts
- `make test` - Run the application (same as run)
- `make dmg` - Create macOS DMG package (macOS only)
- `make help` - Show available targets

## How It Works

The `main.py` file demonstrates:

1. **Window Initialization**: Creates a 1280x800 window with VSync and High DPI support
2. **Resource Directory Search**: The `search_and_set_resource_dir()` function searches for the resources folder in common locations:
   - Current working directory
   - Application directory (where main.py is located)
   - Up to 3 levels above the application directory
3. **Texture Loading**: Loads an image from the resources directory
4. **Game Loop**: Standard raylib game loop with drawing operations
5. **Cleanup**: Properly unloads resources and closes the window

## Customizing

### Changing the Window Size

Edit the `init_window` call in `main.py`:
```python
init_window(1280, 800, "Hello Raylib")
```

### Adding Your Own Resources

1. Place your resource files in the `resources/` directory
2. Load them using raylib functions:
   ```python
   texture = load_texture("your_image.png")
   ```

### Using Your Own Code

Simply replace `main.py` with your own code, or modify the existing file to add your game logic.

## Building for Distribution

### macOS DMG Package

Create a macOS DMG package:
```bash
make dmg
```

### Standalone Executable

To create a standalone executable, you can use tools like PyInstaller:

```bash
uv add pyinstaller
uv run pyinstaller --onefile --windowed main.py
```

## Troubleshooting

### "ModuleNotFoundError: No module named 'pyray'"

Make sure you've installed the dependencies:
```bash
make install
# or
uv sync --no-install-project
```

### "uv: command not found"

Make sure uv is installed. See the [Prerequisites](#prerequisites) section for installation instructions.

### Resource files not found

The `search_and_set_resource_dir()` function will search for the resources folder in several locations. Make sure:
- The `resources/` folder exists in your project directory
- You're running the script from a location where the resources folder can be found

### Import errors

Make sure you're using Python 3.8 or later. Check your Python version:
```bash
python --version
```

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

