local rl = require("raylib")

-- Tell the window to use vsync and work on high DPI displays
-- Note: We use addition for flags which is safe as long as bits don't overlap
rl.SetConfigFlags(rl.FLAG_VSYNC_HINT + rl.FLAG_WINDOW_HIGHDPI)

-- Create the window and OpenGL context
rl.InitWindow(1280, 800, "Hello Raylib")

-- Initialize audio device
rl.InitAudioDevice()


-- Utility function from resource_dir.h to find the resources folder
rl.SetResourceDir("resources")

-- Load a texture from the resources directory
local wabbit = rl.LoadTexture("wabbit_alpha.png")

-- Load music
local music = rl.LoadMusicStream("crystal_cave_track.mp3")
rl.PlayMusicStream(music)

-- State variables
local position = { x = 400, y = 200 }
local velocity = { x = 200, y = 200 }
local rotation = 0

-- game loop
while not rl.WindowShouldClose() do
    -- Update
    rl.UpdateMusicStream(music)
    
    local deltaTime = rl.GetFrameTime()
    
    position.x = position.x + velocity.x * deltaTime
    position.y = position.y + velocity.y * deltaTime
    rotation = rotation + 90 * deltaTime
    
    -- Bounce logic
    if position.x <= 0 or position.x >= rl.GetScreenWidth() then
        velocity.x = velocity.x * -1
    end
    if position.y <= 0 or position.y >= rl.GetScreenHeight() then
        velocity.y = velocity.y * -1
    end

    -- drawing
    rl.BeginDrawing()

    -- Setup the back buffer for drawing (clear color and depth buffers)
    rl.ClearBackground(rl.BLACK)

    -- draw some text using the default font
    rl.DrawText("Hello Raylib", 200, 200, 20, rl.WHITE)

    -- draw our texture to the screen
    -- Using DrawTexturePro to support rotation
    local w = rl.GetTextureWidth(wabbit)
    local h = rl.GetTextureHeight(wabbit)
    local source = rl.Rectangle(0, 0, w, h)
    local dest = rl.Rectangle(position.x + w/2, position.y + h/2, w, h)
    local origin = rl.Vector2(w/2, h/2)
    
    rl.DrawTexturePro(wabbit, source, dest, origin, rotation, rl.WHITE)
    
    -- end the frame and get ready for the next one
    rl.EndDrawing()
end

-- cleanup
rl.UnloadTexture(wabbit)
rl.UnloadMusicStream(music)
rl.CloseAudioDevice()

-- destroy the window and cleanup the OpenGL context
rl.CloseWindow()
