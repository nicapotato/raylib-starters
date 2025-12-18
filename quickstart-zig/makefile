# raylib-quickstart-zig Makefile
# Build targets for raylib-quickstart-zig

.PHONY: help build run clean test dmg dmg-x86 web serve upload upload-x86 upload-web

# Application metadata
APP_NAME = raylib-quickstart-zig
PRODUCT_NAME = raylib-quickstart-zig
BIN_DIR = zig-out/wasm
DMG_NAME = $(APP_NAME).dmg

# Version from project.conf
VERSION := $(shell grep '^VERSION=' project.conf | cut -d= -f2)
ifeq ($(VERSION),)
  VERSION := latest
endif

# S3 Configuration
S3_BUCKET = s3://dev-nicapotato-user-content
S3_BASE_PATH = $(S3_BUCKET)/games/quickstart-zig/$(VERSION)/mac-arm64
S3_BASE_PATH_WASM = $(S3_BUCKET)/games/quickstart-zig/$(VERSION)/wasm

# Default target
help:
	@echo "raylib-quickstart-zig Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  build          - Build the executable (native)"
	@echo "  build-x86      - Build the executable for Intel macOS (x86_64)"
	@echo "  run            - Build and run the application"
	@echo "  clean          - Remove built binaries and Zig build artifacts"
	@echo "  test           - Run the application (same as run)"
	@echo "  dmg            - Create macOS DMG package (Native)"
	@echo "  dmg-x86        - Create macOS DMG package (Intel x86_64)"
	@echo "  web            - Build for Web (WASM)"
	@echo "  serve          - Build and serve Web version"
	@echo "  upload         - Upload Native DMG to S3"
	@echo "  upload-web     - Upload Web Build to S3"
	@echo ""

# Build the executable (Native)
build:
	@echo "Building $(APP_NAME) (Native)..."
	@zig build --prefix zig-out/mac --prefix-exe-dir . -Doptimize=ReleaseSafe

# Build the executable (Intel x86_64)
build-x86:
	@echo "Building $(APP_NAME) (x86_64)..."
	@zig build --prefix zig-out/mac --prefix-exe-dir . -Dtarget=x86_64-macos -Doptimize=ReleaseSafe

# Build and run the application
run:
	@zig build run

# Clean built binaries and Zig build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf zig-out
	@rm -rf zig-cache
	@rm -rf $(APP_NAME).app
	@rm -f $(APP_NAME)*.dmg
	@echo "Clean complete"

# Test target (same as run)
test: run

# Create macOS DMG package
dmg:
	@echo "==== Creating macOS DMG package (Native) ===="
	@if [ "$$(uname)" != "Darwin" ]; then echo "Error: DMG creation is only supported on macOS"; exit 1; fi
	@zig build package --prefix zig-out/mac --prefix-exe-dir . -Doptimize=ReleaseSafe
	@# Rename versioned DMG to standard name if needed
	@ARCH="mac-arm64"; \
	VERSIONED_DMG="$(APP_NAME)-$(VERSION)-$$ARCH.dmg"; \
	if [ -f "$$VERSIONED_DMG" ]; then \
		mv "$$VERSIONED_DMG" "$(DMG_NAME)"; \
		echo "Renamed $$VERSIONED_DMG to $(DMG_NAME)"; \
	fi
	@echo "Generating SHA256 Checksum..."
	@shasum -a 256 "$(DMG_NAME)" > "$(DMG_NAME).sha256"
	@echo "Checksum saved to $(DMG_NAME).sha256"

dmg-x86:
	@echo "==== Creating macOS DMG package (x86_64) ===="
	@if [ "$$(uname)" != "Darwin" ]; then echo "Error: DMG creation is only supported on macOS"; exit 1; fi
	@zig build package --prefix zig-out/mac --prefix-exe-dir . -Dtarget=x86_64-macos -Doptimize=ReleaseSafe
	@ARCH="mac-x86_64"; \
	VERSIONED_DMG="$(APP_NAME)-$(VERSION)-$$ARCH.dmg"; \
	if [ -f "$$VERSIONED_DMG" ]; then \
		mv "$$VERSIONED_DMG" "$(APP_NAME)-x86_64.dmg"; \
	fi

# Web Build
web:
	@echo "Building for Web..."
	@SYSROOT=$$(emcc --cflags | grep -o '\--sysroot=[^ ]*' | cut -d= -f2); \
	if [ -z "$$SYSROOT" ]; then echo "Error: Could not determine emcc sysroot"; exit 1; fi; \
	zig build package -Dtarget=wasm32-emscripten -Doptimize=ReleaseSmall -Dsysroot_path="$$SYSROOT"

# Serve Web Build
serve: web
	@echo "Starting web server at http://localhost:8000/index.html"
	@echo "Press Ctrl+C to stop"
	@open "http://localhost:8000/index.html" || xdg-open "http://localhost:8000/index.html" || true
	@python3 -m http.server 8000 --directory $(BIN_DIR)

# Upload Targets
upload: dmg
	@echo "==== Uploading to S3 (Version: $(VERSION)) ===="
	@if [ ! -f "$(DMG_NAME)" ]; then \
		echo "Error: DMG not found. Please build first."; \
		exit 1; \
	fi
	@echo "Uploading to $(S3_BASE_PATH)/"
	@aws s3 cp "$(DMG_NAME)" "$(S3_BASE_PATH)/"
	@aws s3 cp "$(DMG_NAME).sha256" "$(S3_BASE_PATH)/"
	@echo "Upload complete."

upload-web: web
	@echo "==== Uploading to S3 (Version: $(VERSION)) ===="
	@if [ ! -f "zig-out/wasm/$(APP_NAME)_wasm.zip" ]; then \
		echo "Error: Package not found. Please build first."; \
		exit 1; \
	fi
	@echo "Uploading to $(S3_BASE_PATH_WASM)/"
	@aws s3 cp "zig-out/wasm/$(APP_NAME)_wasm.zip" "$(S3_BASE_PATH_WASM)/"
	@aws s3 cp "zig-out/wasm/index.html" "$(S3_BASE_PATH_WASM)/index.html"
	@aws s3 cp "zig-out/wasm/index.js" "$(S3_BASE_PATH_WASM)/"
	@aws s3 cp "zig-out/wasm/index.wasm" "$(S3_BASE_PATH_WASM)/"
	@if [ -f "zig-out/wasm/index.data" ]; then \
		aws s3 cp "zig-out/wasm/index.data" "$(S3_BASE_PATH_WASM)/"; \
	fi
	@echo "Upload complete."
