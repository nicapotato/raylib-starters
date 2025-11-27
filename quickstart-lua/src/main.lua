local rl = require("raylib")

-- Tell the window to use vsync and work on high DPI displays
-- Note: We use addition for flags which is safe as long as bits don't overlap
rl.SetConfigFlags(rl.FLAG_VSYNC_HINT + rl.FLAG_WINDOW_HIGHDPI)

-- Create the window and OpenGL context
rl.InitWindow(1280, 800, "Hello Raylib")

-- Utility function from resource_dir.h to find the resources folder
rl.SetResourceDir("resources")

-- Load a texture from the resources directory
local wabbit = rl.LoadTexture("wabbit_alpha.png")

-- game loop
while not rl.WindowShouldClose() do
    -- drawing
    rl.BeginDrawing()

    -- Setup the back buffer for drawing (clear color and depth buffers)
    rl.ClearBackground(rl.BLACK)

    -- draw some text using the default font
    rl.DrawText("Hello Raylib", 200, 200, 20, rl.WHITE)

    -- draw our texture to the screen
    rl.DrawTexture(wabbit, 400, 200, rl.WHITE)
    
    -- end the frame and get ready for the next one
    rl.EndDrawing()
end

-- cleanup
rl.UnloadTexture(wabbit)

-- destroy the window and cleanup the OpenGL context
rl.CloseWindow()

