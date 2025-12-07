# Distribution Guide

This project is set up to be distributed on Mac, Windows, and the Web.

## Versioning

The build scripts read the version from `project.conf`.
Make sure to update this file before releasing a new version.

```ini
VERSION=0.0.1
```

All uploads will go to `s3://dev-nicapotato-user-content/games/quickstart-c/<VERSION>/<PLATFORM>/`.

## Mac Distribution (DMG)

The project builds a `.dmg` file for macOS. Since we are not using a paid Apple Developer ID, the app is **ad-hoc signed**.

### Instructions for Users

When you download and run the app for the first time:

1.  **Do NOT double-click.**
2.  **Right-click** (or Control-click) the app and select **Open**.
3.  Click **Open** in the confirmation dialog.

Alternatively, you can go to **System Settings > Privacy & Security** and scroll down to the Security section to click "Open Anyway" for the app.

### Build & Upload

To build the DMG and upload it to S3:

```bash
make dmg
make upload
```

The upload path is versioned based on `project.conf`.
Example: `s3://dev-nicapotato-user-content/games/quickstart-c/0.0.1/mac-arm64/`

---

## Windows Distribution (ZIP)

The project builds a `.zip` file containing `game.exe` and the `resources/` folder.

### Instructions for Users

1.  **Unzip** the downloaded file.
2.  Run `game.exe`.
3.  If Windows SmartScreen appears ("Windows protected your PC"):
    *   Click **More info**.
    *   Click **Run anyway**.

### Build & Upload

Run the `build-release.bat` script. It will:
1.  Compile a static release build.
2.  Create a ZIP file.
3.  Read the version from `project.conf`.
4.  Upload it to S3.

Example: `s3://dev-nicapotato-user-content/games/quickstart-c/0.0.1/windows/`

---

## Web Distribution (WASM)

The project builds a WebAssembly version compatible with modern browsers.

### React Integration

To run this game inside a React app (using an `<iframe>`):

1.  Download the WASM package (ZIP).
2.  Extract the contents (`.html`, `.js`, `.wasm`, `.data`) into your React project's **`public/`** folder (e.g., `public/game/`).
3.  Use an iframe in your component:

    ```jsx
    <iframe 
      src="/game/quickstart-pufferlib.html" 
      width="800" 
      height="450" 
      style={{ border: "none" }}
      title="Raylib Game"
    />
    ```

### Build & Upload

To build the web package and upload it to S3:

```bash
make -f Makefile.web package
make -f Makefile.web upload
```

The upload path is versioned based on `project.conf`.
Example: `s3://dev-nicapotato-user-content/games/quickstart-c/0.0.1/wasm/`

