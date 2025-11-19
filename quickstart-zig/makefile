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
	@rm -f $(DMG_NAME) $(DMG_WRITABLE)
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

