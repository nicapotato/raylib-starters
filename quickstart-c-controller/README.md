# quickstart-c-controller

Raylib (C17) harness for **physical gamepads** on macOS: sticks, triggers, buttons, axis dump, rumble probe. Targets **DualSense**, **DualShock 4**, and Xbox-style controllers through GLFW mapping.

### Desktop (Apple Silicon example)

From this directory:

```bash
make clean
make config=debug_arm64 run
```

Plug a controller **before or after launch**; use **Left / Right arrows** if multiple pads are connected. **`resources/ps3.png` and `resources/xbox.png`** are shipped for the themed layouts.

### Notes

Rerunning `premake5` from `build/` emits a minimal root `Makefile` and **overwrites** the extended one in this repo. Restore `Makefile` from version control afterward, or only regenerate `build/build_files/*.make` then merge by hand.

### Web (experimental)

Wasm builds omit platform gamepads; [`Makefile.web`](Makefile.web) is unchanged from the raylib-quickstart template for optional experiments.
