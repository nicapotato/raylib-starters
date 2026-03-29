#!/usr/bin/env bash
# Build raylib-quickstart-zig.app next to CWD (no DMG). Matches bundle layout in build.zig macOS packaging.
set -euo pipefail

APP_NAME="${APP_NAME:-raylib-quickstart-zig}"
BIN_PATH="${1:-zig-out/bin/${APP_NAME}}"

if [[ "$(uname)" != "Darwin" ]]; then
  echo "Error: app bundle creation is only supported on macOS"
  exit 1
fi
if [[ ! -f "$BIN_PATH" ]]; then
  echo "Error: binary not found: $BIN_PATH"
  exit 1
fi

rm -rf "${APP_NAME}.app"
mkdir -p "${APP_NAME}.app/Contents/MacOS"
mkdir -p "${APP_NAME}.app/Contents/Resources"

cp "$BIN_PATH" "${APP_NAME}.app/Contents/MacOS/$APP_NAME"
chmod +x "${APP_NAME}.app/Contents/MacOS/$APP_NAME"
if [[ -d "resources" ]]; then
  cp -r resources "${APP_NAME}.app/Contents/Resources/"
fi

PLIST="${APP_NAME}.app/Contents/Info.plist"
{
  echo '<?xml version="1.0" encoding="UTF-8"?>'
  echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">'
  echo '<plist version="1.0"><dict>'
  echo "<key>CFBundleExecutable</key><string>$APP_NAME</string>"
  echo "<key>CFBundleIconFile</key><string>raylib</string>"
  echo '</dict></plist>'
} >"$PLIST"

if [[ -f "resources/raylib.ico" ]]; then
  sips -s format icns "resources/raylib.ico" --out "${APP_NAME}.app/Contents/Resources/raylib.icns" 2>/dev/null || true
fi

if security find-identity -v -p codesigning 2>/dev/null | grep -q "Developer ID Application"; then
  SIGN_IDENTITY=$(security find-identity -v -p codesigning | grep "Developer ID Application" | head -1 | sed 's/.*"\(.*\)".*/\1/')
  codesign --force --deep --sign "$SIGN_IDENTITY" --options runtime --timestamp "${APP_NAME}.app" 2>/dev/null || \
    codesign --force --deep --sign "$SIGN_IDENTITY" --options runtime "${APP_NAME}.app" 2>/dev/null || true
else
  codesign --force --deep --sign - --options runtime "${APP_NAME}.app" 2>/dev/null || true
fi

echo "App bundle created: ${APP_NAME}.app"
