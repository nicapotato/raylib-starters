# raylib-quickstart-rust Makefile
# Build targets for raylib-quickstart-rust

.PHONY: help build install run clean test dmg

# Application metadata
APP_NAME = raylib-quickstart-rust
PRODUCT_NAME = raylib-quickstart-rust
PRODUCT_VERSION = 0.1.0
BUNDLE_ID = com.yourname.raylib-quickstart-rust
COPYRIGHT = (c) 2025 YOUR NAME

# Build configuration
BUILD_MODE = release
TARGET_DIR = target/$(BUILD_MODE)
EXECUTABLE = $(TARGET_DIR)/$(APP_NAME)

# macOS app bundle paths
APP_BUNDLE = $(PRODUCT_NAME).app
CONTENTS_DIR = $(APP_BUNDLE)/Contents
MACOS_DIR = $(CONTENTS_DIR)/MacOS
RESOURCES_DIR = $(CONTENTS_DIR)/Resources
DMG_NAME = $(APP_NAME).dmg
DMG_WRITABLE = $(APP_NAME)_writeable.dmg

# Default target
help:
	@echo "raylib-quickstart-rust Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  build          - Build the executable (debug)"
	@echo "  install        - Build the release executable"
	@echo "  run            - Build and run the application"
	@echo "  clean          - Remove built binaries and Rust build artifacts"
	@echo "  test           - Run the application (same as run)"
	@echo "  dmg            - Create macOS DMG package (macOS only)"
	@echo ""
	@echo "Examples:"
	@echo "  make build"
	@echo "  make install"
	@echo "  make run"
	@echo "  make dmg"

# Build the executable (debug)
build:
	@echo "Building $(APP_NAME) (debug)..."
	@cargo build
	@echo "Built: target/debug/$(APP_NAME)"

# Build the release executable
install:
	@echo "Building $(APP_NAME) (release)..."
	@cargo build --release
	@echo "Built: $(EXECUTABLE)"

# Build and run the application
run: build
	@echo "Running $(APP_NAME)..."
	@echo "Working directory: $$(pwd)"
	@cargo run

# Clean built binaries and Rust build artifacts
clean:
	@echo "Cleaning..."
	@cargo clean
	@rm -rf $(APP_BUNDLE)
	@rm -f $(APP_NAME) $(APP_NAME).exe
	@rm -f $(DMG_NAME) $(DMG_WRITABLE)
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
	@if [ ! -f "$(EXECUTABLE)" ]; then \
		echo "Error: Executable not found at $(EXECUTABLE)"; \
		echo "Please build the project first with: make install"; \
		exit 1; \
	fi
	@echo "Building DMG..."
	@# Clean up any existing bundle
	@rm -rf "$(APP_BUNDLE)"
	@# Create bundle structure
	@mkdir -p "$(MACOS_DIR)"
	@mkdir -p "$(RESOURCES_DIR)"
	@# Copy executable
	@cp "$(EXECUTABLE)" "$(MACOS_DIR)/$(APP_NAME)"
	@chmod +x "$(MACOS_DIR)/$(APP_NAME)"
	@# Copy resources
	@if [ -d "resources" ]; then \
		cp -r resources "$(RESOURCES_DIR)/"; \
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

