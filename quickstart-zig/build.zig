const std = @import("std");
const rlz = @import("raylib_zig");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const sysroot_path = b.option([]const u8, "sysroot_path", "Path to emscripten sysroot");

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

    const exe_mod = b.createModule(.{
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });
    exe_mod.addImport("raylib", raylib);

    const exe = if (target.result.os.tag == .emscripten)
        b.addLibrary(.{
            .name = "raylib-quickstart-zig",
            .root_module = exe_mod,
            .linkage = .static,
        })
    else
        b.addExecutable(.{
            .name = "raylib-quickstart-zig",
            .root_module = exe_mod,
        });

    // Web build adjustments for executable/library
    if (target.result.os.tag == .emscripten) {
        if (sysroot_path) |sysroot| {
            exe.addIncludePath(.{ .cwd_relative = b.fmt("{s}/include", .{sysroot}) });
            // We don't add library path here because we link with emcc later
        }

        // We need to install raylib artifact to link it later
        b.installArtifact(raylib_artifact);
    } else {
        // Native specific adjustments if any
    }

    exe.linkLibrary(raylib_artifact);
    b.installArtifact(exe);

    if (target.result.os.tag != .emscripten) {
        const run_cmd = b.addRunArtifact(exe);
        run_cmd.step.dependOn(b.getInstallStep());

        const run_step = b.step("run", "Run the app");
        run_step.dependOn(&run_cmd.step);
    }
}
