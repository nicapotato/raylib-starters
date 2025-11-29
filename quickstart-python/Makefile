# raylib-quickstart-python Makefile
# Build targets for raylib-quickstart-python

.PHONY: help install run clean test dmg

# Application metadata
APP_NAME = raylib-quickstart-python
PRODUCT_NAME = raylib-quickstart-python
PRODUCT_VERSION = 1.0.0
BUNDLE_ID = com.yourname.raylib-quickstart-python
COPYRIGHT = (c) 2025 YOUR NAME
MAIN_FILE = main.py

# macOS app bundle paths
APP_BUNDLE = $(PRODUCT_NAME).app
CONTENTS_DIR = $(APP_BUNDLE)/Contents
MACOS_DIR = $(CONTENTS_DIR)/MacOS
RESOURCES_DIR = $(CONTENTS_DIR)/Resources
DMG_NAME = $(APP_NAME).dmg
DMG_WRITABLE = $(APP_NAME)_writeable.dmg
ICON_ICO = resources/app-icon.ico
ICON_ICNS = resources/app-icon.icns
ICON_NAME = app-icon

# uv paths
UV_VENV = .venv
UV_PYTHON = $(UV_VENV)/bin/python

# Default target
help:
	@echo "raylib-quickstart-python Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  install        - Install dependencies using uv"
	@echo "  run            - Install dependencies and run the application"
	@echo "  clean          - Remove virtual environment and build artifacts"
	@echo "  test           - Run the application (same as run)"
	@echo "  dmg            - Create macOS DMG package (macOS only)"
	@echo ""
	@echo "Examples:"
	@echo "  make install"
	@echo "  make run"
	@echo "  make dmg"

# Check if uv is installed
check-uv:
	@command -v uv >/dev/null 2>&1 || { \
		echo "Error: uv is not installed. Please install it first:"; \
		echo "  curl -LsSf https://astral.sh/uv/install.sh | sh"; \
		echo "  Or visit: https://github.com/astral-sh/uv"; \
		exit 1; \
	}

# Install dependencies using uv
install: check-uv
	@echo "Installing dependencies with uv..."
	@uv sync --no-install-project
	@echo "Dependencies installed"

# Run the application
run: install
	@echo "Copying resources..."
	@cp ../resources/crystal_cave_track.mp3 resources/
	@echo "Running $(APP_NAME)..."
	@# Ensure resources are accessible (they should be in the project root)
	@uv run python $(MAIN_FILE)

# Clean virtual environment and build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(UV_VENV)
	@rm -rf $(APP_BUNDLE)
	@rm -f $(DMG_NAME) $(DMG_WRITABLE) $(ICON_ICNS)
	@rm -rf __pycache__ *.pyc *.pyo
	@rm -rf build dist *.egg-info
	@echo "Clean complete"

# Test target (same as run)
test: run

# Create macOS DMG package
dmg: install
	@echo "==== Creating macOS DMG package ===="
	@if [ "$$(uname)" != "Darwin" ]; then \
		echo "Error: DMG creation is only supported on macOS"; \
		exit 1; \
	fi
	@if [ ! -f "$(UV_PYTHON)" ]; then \
		echo "Error: Virtual environment not found"; \
		echo "Please install dependencies first with: make install"; \
		exit 1; \
	fi
	@echo "Building DMG..."
	@# Clean up any existing bundle
	@rm -rf "$(APP_BUNDLE)"
	@# Create bundle structure
	@mkdir -p "$(MACOS_DIR)"
	@mkdir -p "$(RESOURCES_DIR)"
	@# Create a wrapper script that activates the venv and runs the app
	@echo '#!/bin/bash' > "$(MACOS_DIR)/$(APP_NAME)"
	@echo 'SCRIPT_DIR="$$(cd "$$(dirname "$$0")" && pwd)"' >> "$(MACOS_DIR)/$(APP_NAME)"
	@echo 'RESOURCES_DIR="$$(dirname "$$SCRIPT_DIR")/Resources"' >> "$(MACOS_DIR)/$(APP_NAME)"
	@echo 'cd "$$RESOURCES_DIR"' >> "$(MACOS_DIR)/$(APP_NAME)"
	@echo 'exec "$$RESOURCES_DIR/.venv/bin/python" "$$RESOURCES_DIR/$(MAIN_FILE)"' >> "$(MACOS_DIR)/$(APP_NAME)"
	@chmod +x "$(MACOS_DIR)/$(APP_NAME)"
	@# Copy resources
	@if [ -d "resources" ]; then \
		cp -r resources "$(RESOURCES_DIR)/"; \
		cp ../resources/crystal_cave_track.mp3 "$(RESOURCES_DIR)/resources/"; \
	fi
	@# Copy the entire virtual environment (for standalone distribution)
	@cp -r $(UV_VENV) "$(RESOURCES_DIR)/.venv"
	@# Copy main.py to Resources so the script can find it
	@cp $(MAIN_FILE) "$(RESOURCES_DIR)/"
	@# Convert icon from .ico to .icns for macOS
	@if [ -f "$(ICON_ICO)" ]; then \
		echo "Converting icon to .icns format..."; \
		if sips -s format icns "$(ICON_ICO)" --out "$(ICON_ICNS)" > /dev/null 2>&1; then \
			echo "Icon converted successfully"; \
		else \
			echo "Warning: Direct conversion failed. Creating iconset..."; \
			ICONSET_DIR=$$(mktemp -d)/$(ICON_NAME).iconset; \
			mkdir -p "$$ICONSET_DIR"; \
			trap "rm -rf $$(dirname $$ICONSET_DIR)" EXIT; \
			sips -z 16 16 "$(ICON_ICO)" --out "$$ICONSET_DIR/icon_16x16.png" > /dev/null 2>&1; \
			sips -z 32 32 "$(ICON_ICO)" --out "$$ICONSET_DIR/icon_16x16@2x.png" > /dev/null 2>&1; \
			sips -z 32 32 "$(ICON_ICO)" --out "$$ICONSET_DIR/icon_32x32.png" > /dev/null 2>&1; \
			sips -z 64 64 "$(ICON_ICO)" --out "$$ICONSET_DIR/icon_32x32@2x.png" > /dev/null 2>&1; \
			sips -z 128 128 "$(ICON_ICO)" --out "$$ICONSET_DIR/icon_128x128.png" > /dev/null 2>&1; \
			sips -z 256 256 "$(ICON_ICO)" --out "$$ICONSET_DIR/icon_128x128@2x.png" > /dev/null 2>&1; \
			sips -z 256 256 "$(ICON_ICO)" --out "$$ICONSET_DIR/icon_256x256.png" > /dev/null 2>&1; \
			sips -z 512 512 "$(ICON_ICO)" --out "$$ICONSET_DIR/icon_512x512@2x.png" > /dev/null 2>&1; \
			sips -z 1024 1024 "$(ICON_ICO)" --out "$$ICONSET_DIR/icon_512x512@2x.png" > /dev/null 2>&1; \
			if iconutil -c icns "$$ICONSET_DIR" -o "$(ICON_ICNS)" 2>/dev/null; then \
				echo "Icon converted via iconset"; \
			else \
				echo "Warning: Icon conversion failed, app will use default icon"; \
			fi; \
			rm -rf "$$(dirname $$ICONSET_DIR)"; \
		fi; \
		if [ -f "$(ICON_ICNS)" ]; then \
			cp "$(ICON_ICNS)" "$(RESOURCES_DIR)/$(ICON_NAME).icns"; \
			echo "Icon copied to app bundle"; \
		fi; \
	fi
	@# Create Info.plist
	@echo '<?xml version="1.0" encoding="UTF-8"?>' > "$(CONTENTS_DIR)/Info.plist"
	@echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '<plist version="1.0">' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '<dict>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<key>CFBundleExecutable</key>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<string>$(APP_NAME)</string>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<key>CFBundleIdentifier</key>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<string>$(BUNDLE_ID)</string>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<key>CFBundleName</key>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<string>$(PRODUCT_NAME)</string>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<key>CFBundleVersion</key>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<string>$(PRODUCT_VERSION)</string>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<key>CFBundleShortVersionString</key>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<string>$(PRODUCT_VERSION)</string>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<key>CFBundlePackageType</key>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<string>APPL</string>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<key>CFBundleInfoDictionaryVersion</key>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<string>6.0</string>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<key>LSMinimumSystemVersion</key>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<string>10.9</string>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<key>NSHumanReadableCopyright</key>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '	<string>$(COPYRIGHT)</string>' >> "$(CONTENTS_DIR)/Info.plist"
	@if [ -f "$(RESOURCES_DIR)/$(ICON_NAME).icns" ]; then \
		echo '	<key>CFBundleIconFile</key>' >> "$(CONTENTS_DIR)/Info.plist"; \
		echo '	<string>$(ICON_NAME)</string>' >> "$(CONTENTS_DIR)/Info.plist"; \
	fi
	@echo '</dict>' >> "$(CONTENTS_DIR)/Info.plist"
	@echo '</plist>' >> "$(CONTENTS_DIR)/Info.plist"
	@# Clean up any existing DMG files
	@rm -f "$(DMG_NAME)" "$(DMG_WRITABLE)"
	@# Create writable DMG
	@echo "Creating DMG image..."
	@hdiutil create -size 64m -fs HFS+ -volname '$(PRODUCT_NAME)' "$(DMG_WRITABLE)" > /dev/null 2>&1 || \
		hdiutil create -size 64m -fs HFS+ -volname '$(PRODUCT_NAME)' "$(DMG_WRITABLE)"
	@# Attach DMG and get disk number
	@ATTACH_OUTPUT=$$(hdiutil attach "$(DMG_WRITABLE)" 2>&1); \
	DISK_NUM=$$(echo "$$ATTACH_OUTPUT" | grep -o '/dev/disk[0-9]*' | head -1); \
	VOLUME_PATH="/Volumes/$(PRODUCT_NAME)"; \
	sleep 2; \
	cp -r "$(APP_BUNDLE)" "$$VOLUME_PATH/"; \
	ln -s /Applications "$$VOLUME_PATH/Applications"; \
	hdiutil detach "$$DISK_NUM" > /dev/null 2>&1; \
	echo "Converting to compressed DMG..."; \
	hdiutil convert "$(DMG_WRITABLE)" -format UDZO -o "$(DMG_NAME)" > /dev/null 2>&1 || hdiutil convert "$(DMG_WRITABLE)" -format UDZO -o "$(DMG_NAME)"; \
	rm -f "$(DMG_WRITABLE)"; \
	echo "DMG created successfully: $(DMG_NAME)"
