# raylib-quickstart-zig Makefile
# Build targets for raylib-quickstart-zig

.PHONY: help build run clean test dmg dmg-x86 web serve upload upload-x86 upload-web

# Application metadata
APP_NAME = raylib-quickstart-zig
PRODUCT_NAME = raylib-quickstart-zig
BIN_DIR = zig-out/bin

# Version from project.conf
VERSION := $(shell grep '^VERSION=' project.conf | cut -d= -f2)
ifeq ($(VERSION),)
  VERSION := latest
endif

# S3 Configuration
S3_BUCKET = s3://dev-nicapotato-user-content
S3_BASE_PATH = $(S3_BUCKET)/games/$(APP_NAME)/$(VERSION)

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
	@echo "  upload-x86     - Upload Intel DMG to S3"
	@echo "  upload-web     - Upload Web Build to S3"
	@echo ""

# Build the executable (Native)
build:
	@echo "Building $(APP_NAME) (Native)..."
	@zig build -Doptimize=ReleaseSafe

# Build the executable (Intel x86_64)
build-x86:
	@echo "Building $(APP_NAME) (x86_64)..."
	@zig build -Dtarget=x86_64-macos -Doptimize=ReleaseSafe

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
	@zig build package -Doptimize=ReleaseSafe

dmg-x86:
	@echo "==== Creating macOS DMG package (x86_64) ===="
	@if [ "$$(uname)" != "Darwin" ]; then echo "Error: DMG creation is only supported on macOS"; exit 1; fi
	@zig build package -Dtarget=x86_64-macos -Doptimize=ReleaseSafe

# Web Build
web:
	@echo "Building for Web..."
	@SYSROOT=$$(emcc --cflags | grep -o '\--sysroot=[^ ]*' | cut -d= -f2); \
	if [ -z "$$SYSROOT" ]; then echo "Error: Could not determine emcc sysroot"; exit 1; fi; \
	zig build package -Dtarget=wasm32-emscripten -Doptimize=ReleaseSmall -Dsysroot_path="$$SYSROOT"

# Serve Web Build
serve: web
	@echo "Starting web server at http://localhost:8000/$(APP_NAME).html"
	@echo "Press Ctrl+C to stop"
	@open "http://localhost:8000/$(APP_NAME).html" || xdg-open "http://localhost:8000/$(APP_NAME).html" || true
	@python3 -m http.server 8000 --directory $(BIN_DIR)

# Upload Targets
upload: dmg
	@echo "==== Uploading to S3 (Version: $(VERSION)) ===="
	@ARCH="mac-arm64"; \
	DMG_FILE="$(APP_NAME)-$(VERSION)-$$ARCH.dmg"; \
	if [ -f "zig-out/bin/$$DMG_FILE" ]; then \
		echo "Uploading $$DMG_FILE to $(S3_BASE_PATH)/$$ARCH/"; \
		aws s3 cp "zig-out/bin/$$DMG_FILE" "$(S3_BASE_PATH)/$$ARCH/$(APP_NAME).dmg"; \
	elif [ -f "$$DMG_FILE" ]; then \
		echo "Uploading $$DMG_FILE to $(S3_BASE_PATH)/$$ARCH/"; \
		aws s3 cp "$$DMG_FILE" "$(S3_BASE_PATH)/$$ARCH/$(APP_NAME).dmg"; \
	else \
		echo "Error: $$DMG_FILE not found."; \
		exit 1; \
	fi

upload-x86: dmg-x86
	@echo "==== Uploading to S3 (Version: $(VERSION)) ===="
	@ARCH="mac-x86_64"; \
	DMG_FILE="$(APP_NAME)-$(VERSION)-$$ARCH.dmg"; \
	if [ -f "zig-out/bin/$$DMG_FILE" ]; then \
		echo "Uploading $$DMG_FILE to $(S3_BASE_PATH)/$$ARCH/"; \
		aws s3 cp "zig-out/bin/$$DMG_FILE" "$(S3_BASE_PATH)/$$ARCH/$(APP_NAME).dmg"; \
	elif [ -f "$$DMG_FILE" ]; then \
		echo "Uploading $$DMG_FILE to $(S3_BASE_PATH)/$$ARCH/"; \
		aws s3 cp "$$DMG_FILE" "$(S3_BASE_PATH)/$$ARCH/$(APP_NAME).dmg"; \
	else \
		echo "Error: $$DMG_FILE not found."; \
		exit 1; \
	fi

upload-web: web
	@echo "==== Uploading to S3 (Version: $(VERSION)) ===="
	@if [ ! -f "zig-out/bin/$(APP_NAME)_wasm.zip" ]; then \
		echo "Error: Package not found. Please build first."; \
		exit 1; \
	fi
	@echo "Uploading to $(S3_BASE_PATH)/wasm/"
	@aws s3 cp "zig-out/bin/$(APP_NAME)_wasm.zip" "$(S3_BASE_PATH)/wasm/"
	@aws s3 cp "zig-out/bin/$(APP_NAME).html" "$(S3_BASE_PATH)/wasm/index.html"
	@aws s3 cp "zig-out/bin/$(APP_NAME).js" "$(S3_BASE_PATH)/wasm/"
	@aws s3 cp "zig-out/bin/$(APP_NAME).wasm" "$(S3_BASE_PATH)/wasm/"
	@if [ -f "zig-out/bin/$(APP_NAME).data" ]; then \
		aws s3 cp "zig-out/bin/$(APP_NAME).data" "$(S3_BASE_PATH)/wasm/"; \
	fi
	@echo "Upload complete."
