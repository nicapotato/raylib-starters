// Raylib example file.
// This is an example main file for a simple raylib project.
// Use this as a starting point or replace it with your code.
//
// by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

const std = @import("std");
const rl = @import("raylib");

pub fn main() !void {
    // Tell the window to use vsync and work on high DPI displays
    rl.setConfigFlags(rl.ConfigFlags{ .vsync_hint = true, .window_highdpi = true });

    // Create the window and OpenGL context
    rl.initWindow(1280, 800, "Hello Raylib");
    defer rl.closeWindow(); // Close window and OpenGL context

    // Set working directory to resources folder if it exists
    // For macOS app bundles, resources are in Contents/Resources/resources/
    // For development builds, resources are copied next to the executable
    searchAndSetResourceDir() catch {};

    // Load a texture from the resources directory
    const wabbit = rl.loadTexture("wabbit_alpha.png") catch |err| {
        std.debug.print("Error: Could not load texture 'wabbit_alpha.png': {}\n", .{err});
        std.debug.print("Please ensure the resources directory exists and contains wabbit_alpha.png\n", .{});
        return;
    };
    defer rl.unloadTexture(wabbit);

    rl.setTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Game loop
    while (!rl.windowShouldClose()) { // Run the loop until the user presses ESCAPE or presses the Close button on the window
        // Drawing
        rl.beginDrawing();
        defer rl.endDrawing();

        // Setup the back buffer for drawing (clear color and depth buffers)
        rl.clearBackground(rl.Color.black);

        // Draw some text using the default font
        rl.drawText("Hello Raylib", 200, 200, 20, rl.Color.white);

        // Draw our texture to the screen
        rl.drawTexture(wabbit, 400, 200, rl.Color.white);
    }
}

// searchAndSetResourceDir searches for the resources folder and sets it as the current working directory
fn searchAndSetResourceDir() !void {
    // Web: resources are preloaded at root or handled by emscripten virtual fs
    if (@import("builtin").os.tag == .emscripten) {
        // Emscripten: Resources are usually preloaded into /resources
        std.posix.chdir("resources") catch {};
        return;
    }

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // Check current directory first
    if (std.fs.cwd().openDir("resources", .{})) |_| {
        std.posix.chdir("resources") catch {};
        return;
    } else |_| {}

    // Try to get executable path
    const exe_path = try std.fs.selfExePathAlloc(allocator);
    defer allocator.free(exe_path);

    if (std.fs.path.dirname(exe_path)) |exe_dir| {
        // Check next to executable (development build)
        const resources_path = try std.fmt.allocPrint(allocator, "{s}/resources", .{exe_dir});
        defer allocator.free(resources_path);
        
        if (std.fs.openDirAbsolute(resources_path, .{})) |_| {
            std.posix.chdir(resources_path) catch {};
            return;
        } else |_| {}

        // Check macOS app bundle Resources directory
        const bundle_resources_path = try std.fmt.allocPrint(allocator, "{s}/../Resources/resources", .{exe_dir});
        defer allocator.free(bundle_resources_path);
        
        if (std.fs.openDirAbsolute(bundle_resources_path, .{})) |_| {
            std.posix.chdir(bundle_resources_path) catch {};
            return;
        } else |_| {}
    }
}

