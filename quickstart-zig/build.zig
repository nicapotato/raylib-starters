const std = @import("std");
const rlz = @import("raylib_zig");

pub fn build(b: *std.Build) !void {
    // 1. Project Configuration
    const version = try getProjectVersion(b.allocator, "project.conf");
    const app_name = "raylib-quickstart-zig";

    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // Options
    const sysroot_path = b.option([]const u8, "sysroot_path", "Path to emscripten sysroot");

    // 2. Define Raylib Dependency
    const raylib_dep = b.dependency("raylib_zig", .{
        .target = target,
        .optimize = optimize,
    });
    const raylib = raylib_dep.module("raylib");
    const raylib_artifact = raylib_dep.artifact("raylib");

    // Web build adjustments for raylib library
    if (target.result.os.tag == .emscripten) {
        if (sysroot_path) |sysroot| {
            const include_path = b.fmt("{s}/include", .{sysroot});
            raylib_artifact.addIncludePath(.{ .cwd_relative = include_path });
            raylib_artifact.addLibraryPath(.{ .cwd_relative = b.fmt("{s}/lib/wasm32-emscripten", .{sysroot}) });
        }
    }

    // 3. Define Executable
    const exe_mod = b.createModule(.{
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });
    exe_mod.addImport("raylib", raylib);

    // For Web, we build a static library first, then link with emcc
    const exe = if (target.result.os.tag == .emscripten)
        b.addLibrary(.{
            .name = app_name,
            .root_module = exe_mod,
            .linkage = .static,
        })
    else
        b.addExecutable(.{
            .name = app_name,
            .root_module = exe_mod,
        });

    if (target.result.os.tag == .emscripten) {
        if (sysroot_path) |sysroot| {
            exe.addIncludePath(.{ .cwd_relative = b.fmt("{s}/include", .{sysroot}) });
        }
        b.installArtifact(raylib_artifact); // Install libraylib.a
    } else {
        // Copy resources for development run
        const install_resources = b.addInstallDirectory(.{
            .source_dir = b.path("resources"),
            .install_dir = .bin,
            .install_subdir = "resources",
        });
        exe.step.dependOn(&install_resources.step);

        const install_mp3 = b.addInstallFile(b.path("../resources/crystal_cave_track.mp3"), "bin/resources/crystal_cave_track.mp3");
        exe.step.dependOn(&install_mp3.step);
    }

    exe.linkLibrary(raylib_artifact);
    b.installArtifact(exe);

    // 4. Run Step
    if (target.result.os.tag != .emscripten) {
        const run_cmd = b.addRunArtifact(exe);
        run_cmd.step.dependOn(b.getInstallStep());
        run_cmd.cwd = .{ .cwd_relative = b.getInstallPath(.bin, "") };
        if (b.args) |args| run_cmd.addArgs(args);
        const run_step = b.step("run", "Run the app");
        run_step.dependOn(&run_cmd.step);
    }

    // ============================================================
    // CUSTOM STEPS: Package
    // ============================================================

    const package_step = b.step("package", "Package the application (DMG for Mac, Zip for Web)");

    if (target.result.os.tag == .emscripten) {
        // --- WEB PACKAGING ---
        // 1. Link with EMCC
        const emcc_cmd = b.addSystemCommand(&[_][]const u8{"emcc"});
        emcc_cmd.addFileArg(exe.getEmittedBin()); // The .a file from Zig
        emcc_cmd.addArg("-o");
        const out_html = b.fmt("zig-out/bin/{s}.html", .{app_name});
        emcc_cmd.addArg(out_html);

        // Add Raylib library
        emcc_cmd.addArg("-Lzig-out/lib");
        emcc_cmd.addArg("-lraylib");

        // Emscripten Flags
        emcc_cmd.addArgs(&[_][]const u8{
            "-s",             "USE_GLFW=3",
            "-s",             "WASM=1",
            "-s",             "ALLOW_MEMORY_GROWTH=1",
            "-s",             "ASYNCIFY",
            "-s",             "USE_WEBGL2=1",
            "--preload-file", "resources",
            "--preload-file", "../resources/crystal_cave_track.mp3@resources/crystal_cave_track.mp3",
            "--shell-file",   "minshell.html",
            "-Os", // Optimize for size
        });

        // Create output directory step
        const mkdir_cmd = b.addSystemCommand(&[_][]const u8{ "mkdir", "-p", "zig-out/bin" });
        emcc_cmd.step.dependOn(&mkdir_cmd.step);

        emcc_cmd.step.dependOn(b.getInstallStep());
        package_step.dependOn(&emcc_cmd.step);

        // 2. Zip the output
        const zip_cmd = b.addSystemCommand(&[_][]const u8{
            "zip",                          "-r",                         b.fmt("{s}_wasm.zip", .{app_name}),
            b.fmt("{s}.html", .{app_name}), b.fmt("{s}.js", .{app_name}), b.fmt("{s}.wasm", .{app_name}),
            b.fmt("{s}.data", .{app_name}),
        });
        zip_cmd.cwd = b.path("zig-out/bin");
        zip_cmd.step.dependOn(&emcc_cmd.step);
        package_step.dependOn(&zip_cmd.step);
    } else if (target.result.os.tag == .macos) {
        // --- MACOS PACKAGING (DMG) ---
        const arch_str = if (target.result.cpu.arch == .x86_64) "mac-x86_64" else "mac-arm64";
        const dmg_name = b.fmt("{s}-{s}-{s}.dmg", .{ app_name, version, arch_str });

        // Script to create DMG
        const create_dmg_script =
            \\set -e
            \\APP_NAME="$1"
            \\DMG_NAME="$2"
            \\VOL_NAME="$3"
            \\
            \\rm -rf "$APP_NAME.app" "$DMG_NAME"
            \\mkdir -p "$APP_NAME.app/Contents/MacOS"
            \\mkdir -p "$APP_NAME.app/Contents/Resources"
            \\
            \\cp "zig-out/bin/$APP_NAME" "$APP_NAME.app/Contents/MacOS/"
            \\chmod +x "$APP_NAME.app/Contents/MacOS/$APP_NAME"
            \\if [ -d "resources" ]; then cp -r resources "$APP_NAME.app/Contents/Resources/"; fi
            \\cp ../resources/crystal_cave_track.mp3 "$APP_NAME.app/Contents/Resources/resources/"
            \\
            \\# Create Info.plist (simplified)
            \\PLIST="$APP_NAME.app/Contents/Info.plist"
            \\echo '<?xml version="1.0" encoding="UTF-8"?>' > "$PLIST"
            \\echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> "$PLIST"
            \\echo '<plist version="1.0"><dict>' >> "$PLIST"
            \\echo "<key>CFBundleExecutable</key><string>$APP_NAME</string>" >> "$PLIST"
            \\echo "<key>CFBundleIconFile</key><string>app-icon</string>" >> "$PLIST"
            \\echo '</dict></plist>' >> "$PLIST"
            \\
            \\# Icon conversion (if exists)
            \\if [ -f "resources/app-icon.ico" ]; then
            \\    sips -s format icns "resources/app-icon.ico" --out "$APP_NAME.app/Contents/Resources/app-icon.icns" || true
            \\fi
            \\
            \\hdiutil create -size 32m -fs HFS+ -volname "$VOL_NAME" "tmp.dmg"
            \\hdiutil attach "tmp.dmg" -mountpoint "/Volumes/$VOL_NAME"
            \\cp -r "$APP_NAME.app" "/Volumes/$VOL_NAME/"
            \\ln -s /Applications "/Volumes/$VOL_NAME/Applications"
            \\hdiutil detach "/Volumes/$VOL_NAME"
            \\hdiutil convert "tmp.dmg" -format UDZO -o "$DMG_NAME"
            \\rm "tmp.dmg"
            \\rm -rf "$APP_NAME.app"
        ;

        const dmg_cmd = b.addSystemCommand(&[_][]const u8{ "sh", "-c", create_dmg_script, "--", app_name, dmg_name, app_name });
        dmg_cmd.step.dependOn(b.getInstallStep());
        package_step.dependOn(&dmg_cmd.step);
    }
}

// Helper to read version from project.conf
fn getProjectVersion(allocator: std.mem.Allocator, file_path: []const u8) ![]const u8 {
    const file = try std.fs.cwd().openFile(file_path, .{});
    defer file.close();

    const content = try file.readToEndAlloc(allocator, 1024);
    defer allocator.free(content);

    var it = std.mem.splitScalar(u8, content, '\n');
    while (it.next()) |line| {
        if (std.mem.startsWith(u8, line, "VERSION=")) {
            const v = std.mem.trim(u8, line["VERSION=".len..], " \r\t");
            return allocator.dupe(u8, v);
        }
    }
    return "0.0.0"; // Default
}
