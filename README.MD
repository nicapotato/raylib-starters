# raylib-starters

Starter templates for small games and tools built with **[raylib](https://www.raylib.com/)** — a minimal C library for graphics, input, audio, and windowing. Each folder under this repo is a self-contained sample you can copy or extend.

## Raylib

Raylib wraps platform glue (window, graphics context, input, audio) behind a simple C API. You draw with textures, shapes, and shaders; desktop builds typically use OpenGL and GLFW under the hood. The same ideas carry over to WebAssembly builds where the project opts into Emscripten.

## Stack (how these projects are put together)

- **Language-first templates** — C and C++ raylib samples live beside bindings-style starters (**Lua**, **Python**, **Rust**, **Zig**, **Go**) so you can pick the layer you want.
- **C quickstarts** — plain **C99**, **clang**/**GCC**, project-local **Makefile** workflows, assets under `resources/`, and optional **fragment shaders** where the demo needs them.
- **Extensions where the sample demands it** — e.g. **Aseprite** import helpers or **gamepad** probing in the dedicated controller template, without pulling that complexity into the minimal C baseline.

## What’s in the sample apps

- **`quickstart-c/`** — Flagship **C** demo: a tiny **menu → fullscreen shader** flow (bouncing sprite, wave shader, optional background music when the asset is present). Good default for “how does raylib + resources + shaders hang together?”
- **`quickstart-c-aesprite/`** — Same family extended with **Aseprite** sheets/tags as extra app states.
- **`quickstart-c-controller/`** — **Gamepad** input probe UI (adapted from raylib’s gamepad example).
- **`quickstart-c-pufferlib/`**, **`quickstart-cpp/`**, **`quickstart-go/`**, **`quickstart-lua/`**, **`quickstart-python/`**, **`quickstart-rust/`**, **`quickstart-zig/`** — Parallel starters for other languages and experiments.

Shared media can sit under [`resources/`](resources/) at the repo root when samples reuse tracks or textures.
