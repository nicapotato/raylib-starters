# raylib-starters

Templates and CI for small [raylib](https://www.raylib.com/) projects.

## Quickstart C — CI/CD

The sample project lives under [`quickstart-c/`](quickstart-c/). Builds are **manual** (`workflow_dispatch` only): run from the Actions tab or with the GitHub CLI.

### Full release (all targets, S3 catalog, itch)

- Workflow: [`quickstart-c.yml`](.github/workflows/quickstart-c.yml)
- Badge: [![Quickstart C full](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c.yml/badge.svg)](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c.yml)

```bash
gh workflow run quickstart-c.yml --ref "$(git branch --show-current)" -f build_platform=all
# or, from this repo with GNU make:
make ga-quickstart-c-all
```

### Per-target builds (one badge per project / platform / CPU)

GitHub only loads workflow files as **`.github/workflows/<name>.yml`** at the root of that folder (no subdirectories). Wrapper files use the `quickstart-c-*.yml` prefix next to [`quickstart-c.yml`](.github/workflows/quickstart-c.yml) (reusable `workflow_call` must stay in that flat path too). Each row is a **separate** workflow so the status badge reflects **that** build only.

| Project | Platform | CPU / arch | Status |
|--------|----------|------------|--------|
| quickstart-c | Web | WASM | [![WASM](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-web.yml/badge.svg)](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-web.yml) |
| quickstart-c | macOS | arm64 | [![macOS arm64](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-macos-arm64.yml/badge.svg)](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-macos-arm64.yml) |
| quickstart-c | macOS | x86_64 | [![macOS x86_64](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-macos-x86_64.yml/badge.svg)](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-macos-x86_64.yml) |
| quickstart-c | Linux | x86_64 | [![Linux x86_64](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-linux-x86_64.yml/badge.svg)](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-linux-x86_64.yml) |
| quickstart-c | Linux | arm64 | [![Linux arm64](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-linux-arm64.yml/badge.svg)](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-linux-arm64.yml) |
| quickstart-c | Windows | x86_64 | [![Windows x86_64](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-windows-x86_64.yml/badge.svg)](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-windows-x86_64.yml) |
| quickstart-c | Windows | arm64 | [![Windows arm64](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-windows-arm64.yml/badge.svg)](https://github.com/nicapotato/raylib-starters/actions/workflows/quickstart-c-windows-arm64.yml) |

Thin workflows call the same implementation ([`quickstart-c.yml`](.github/workflows/quickstart-c.yml)) with the matching `build_platform` input. Example:

```bash
gh workflow run quickstart-c-linux-arm64.yml --ref "$(git branch --show-current)"
```

Release **version** always comes from [`quickstart-c/project.conf`](quickstart-c/project.conf) (`VERSION=…`). Optional **`channel`** (HTML5 / `web` push only) when you run **`quickstart-c.yml`** directly:

```bash
make ga-quickstart-c GA_BUILD_PLATFORM=web GA_CHANNEL=web
```

Full `build_platform=all` pushes HTML5 to the `web` channel and each desktop `.zip` to its own channel: `macos-arm64`, `macos-x86-64`, `linux-x86-64`, `linux-arm64`, `windows-x86-64`, `windows-arm64`. (The WASM zip is still published to S3 for the catalog; it is not duplicated on itch.)
