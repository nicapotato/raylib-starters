# raylib-quickstart-zig Makefile
# Build targets for raylib-quickstart-zig

.PHONY: help build run clean test dmg

# Application metadata
APP_NAME = raylib-quickstart-zig
PRODUCT_NAME = raylib-quickstart-zig
PRODUCT_VERSION = 1.0.0
BUNDLE_ID = com.yourname.raylib-quickstart-zig
COPYRIGHT = (c) 2025 YOUR NAME
BIN_DIR = zig-out/bin

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

# Default target
help:
	@echo "raylib-quickstart-zig Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  build          - Build the executable"
	@echo "  run            - Build and run the application"
	@echo "  clean          - Remove built binaries and Zig build artifacts"
	@echo "  test           - Run the application (same as run)"
	@echo "  dmg            - Create macOS DMG package (macOS only)"
	@echo ""
	@echo "Examples:"
	@echo "  make build"
	@echo "  make run"
	@echo "  make dmg"

# Build the executable
build:
	@echo "Building $(APP_NAME)..."
	@zig build
	@# Copy resources to build output directory
	@if [ -d "resources" ]; then \
		mkdir -p $(BIN_DIR)/resources; \
		cp -r resources/* $(BIN_DIR)/resources/; \
	fi
	@echo "Built: $(BIN_DIR)/$(APP_NAME)"

# Build and run the application
run: build
	@echo "Running $(APP_NAME)..."
	@cd $(BIN_DIR) && ./$(APP_NAME)

# Clean built binaries and Zig build artifacts
clean:
	@echo "Cleaning..."
	@zig build clean
	@rm -rf zig-out
	@rm -rf zig-cache
	@rm -rf $(APP_BUNDLE)
	@rm -f $(APP_NAME) $(APP_NAME).exe
	@rm -f $(DMG_NAME) $(DMG_WRITABLE) $(ICON_ICNS)
	@echo "Clean complete"

# Test target (same as run)
test: run

# Create macOS DMG package
dmg: build
	@echo "==== Creating macOS DMG package ===="
	@if [ "$$(uname)" != "Darwin" ]; then \
		echo "Error: DMG creation is only supported on macOS"; \
		exit 1; \
	fi
	@if [ ! -f "$(BIN_DIR)/$(APP_NAME)" ]; then \
		echo "Error: Executable not found at $(BIN_DIR)/$(APP_NAME)"; \
		echo "Please build the project first with: make build"; \
		exit 1; \
	fi
	@echo "Building DMG..."
	@# Clean up any existing bundle
	@rm -rf "$(APP_BUNDLE)"
	@# Create bundle structure
	@mkdir -p "$(MACOS_DIR)"
	@mkdir -p "$(RESOURCES_DIR)"
	@# Copy executable
	@cp "$(BIN_DIR)/$(APP_NAME)" "$(MACOS_DIR)/$(APP_NAME)"
	@chmod +x "$(MACOS_DIR)/$(APP_NAME)"
	@# Copy resources
	@if [ -d "resources" ]; then \
		cp -r resources "$(RESOURCES_DIR)/"; \
	fi
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
			sips -z 512 512 "$(ICON_ICO)" --out "$$ICONSET_DIR/icon_256x256@2x.png" > /dev/null 2>&1; \
			sips -z 512 512 "$(ICON_ICO)" --out "$$ICONSET_DIR/icon_512x512.png" > /dev/null 2>&1; \
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
	@hdiutil create -size 32m -fs HFS+ -volname '$(PRODUCT_NAME)' "$(DMG_WRITABLE)" > /dev/null 2>&1 || \
		hdiutil create -size 32m -fs HFS+ -volname '$(PRODUCT_NAME)' "$(DMG_WRITABLE)"
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

