#include "connect4.h"
#include "puffernet.h"
#include "time.h"
#include "connect4_app.h"

const unsigned char NOOP_ACTION = 8;

static Weights* weights = NULL;
static LinearLSTM* net = NULL;
static CConnect4 env;
static float observations[42] = {0};
static int actions[1] = {0};
static int tick = 0;
static bool initialized = false;
static int gameMode = 0; // 0 = AI vs AI, 1 = Player vs AI

// Game Over State
static bool gameOver = false;
static int winner = 0; // 0 = None, 1 = Player (Blue), -1 = AI (Red), 2 = Draw

// AI Delay Logic
static bool aiTurnPending = false;
static float aiDelayTimer = 0.0f;
static const float AI_DELAY_DURATION = 0.5f; // 0.5 seconds delay

void InitConnect4(int mode) {
    if (initialized) {
        // partial reset if re-initing with different mode, but usually Unload is called first
        gameMode = mode;
        c_reset(&env);
        gameOver = false;
        winner = 0;
        aiTurnPending = false;
        aiDelayTimer = 0.0f;
        return;
    }

    gameMode = mode;
    gameOver = false;
    winner = 0;
    aiTurnPending = false;
    aiDelayTimer = 0.0f;

    // Load weights from resources directory (handled by SearchAndSetResourceDir in main.c)
    weights = load_weights("connect4_weights.bin", 138632);
    int logit_sizes[] = {7};
    net = make_linearlstm(weights, 1, 42, logit_sizes, 1);

    allocate_cconnect4(&env);
    c_reset(&env);
    
    initialized = true;
}

void CheckGameOver() {
    if (env.terminals[0] == DONE) {
        gameOver = true;
        if (env.rewards[0] == PLAYER_WIN) winner = 1;
        else if (env.rewards[0] == ENV_WIN) winner = -1;
        else winner = 2; 
    }
}

void UpdateConnect4() {
    if (!initialized) return;

    // Handle Top Menu Buttons logic first
    // This should be clickable even during game over (e.g. Reset/New Game)
    // Area: Top 100px
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && mouse.y < 100) {
        // Toggle Mode Button
        Rectangle modeBtn = { 10, 10, 200, 40 };
        if (CheckCollisionPointRec(mouse, modeBtn)) {
            gameMode = !gameMode;
            c_reset(&env); // Reset game on mode switch
            gameOver = false;
            winner = 0;
            aiTurnPending = false;
            aiDelayTimer = 0.0f;
            return;
        }
        
        // Reset / New Game Button
        Rectangle resetBtn = { 220, 10, 100, 40 };
        if (CheckCollisionPointRec(mouse, resetBtn)) {
            c_reset(&env);
            gameOver = false;
            winner = 0;
            aiTurnPending = false;
            aiDelayTimer = 0.0f;
            return;
        }
    }

    // If game is over, we stop processing board/AI updates, 
    // but we let the loop continue so top buttons work.
    if (gameOver) {
        return; 
    }

    // AI Delay Handling (Player vs AI mode)
    if (aiTurnPending) {
        aiDelayTimer += GetFrameTime();
        if (aiDelayTimer >= AI_DELAY_DURATION) {
            c_step_env(&env);
            CheckGameOver();
            aiTurnPending = false;
            aiDelayTimer = 0.0f;
        }
        return; // Wait for AI to move
    }

    env.actions[0] = NOOP_ACTION;
    
    bool playerMoved = false;

    // Player Input Logic
    int playerAction = -1;
    
    // Check Touch/Mouse click on Board
    // Board area logic duplicated from c_render
    int board_height = ROWS * PIECE_HEIGHT;
    int board_width = COLUMNS * PIECE_WIDTH;
    int start_y = (GetScreenHeight() - board_height) / 2 + 50; 
    int start_x = (GetScreenWidth() - board_width) / 2;
    
    // Only allow input in Player vs AI mode
    if (gameMode == 1) {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && mouse.y >= start_y) {
            if (mouse.x >= start_x && mouse.x < start_x + board_width) {
                int col = (int)((mouse.x - start_x) / PIECE_WIDTH);
                if (col >= 0 && col < 7) {
                    playerAction = col;
                }
            }
        }
    }

    // Keyboard Input Fallback
    // Only active in Player vs AI mode
    bool inputActive = (gameMode == 1);

    if (inputActive) {
        if(IsKeyPressed(KEY_ONE)) playerAction = 0;
        if(IsKeyPressed(KEY_TWO)) playerAction = 1;
        if(IsKeyPressed(KEY_THREE)) playerAction = 2;
        if(IsKeyPressed(KEY_FOUR)) playerAction = 3;
        if(IsKeyPressed(KEY_FIVE)) playerAction = 4;
        if(IsKeyPressed(KEY_SIX)) playerAction = 5;
        if(IsKeyPressed(KEY_SEVEN)) playerAction = 6;
    }

    if (playerAction != -1) {
        env.actions[0] = playerAction;
        playerMoved = true;
    } else if (gameMode == 0 && tick % 30 == 0) {
        // AI vs AI logic (every 30 ticks ~ 0.5s)
        for (int i = 0; i < 42; i++) {
            observations[i] = env.observations[i];
        }
        forward_linearlstm(net, (float*)&observations, (int*)&actions);
        env.actions[0] = actions[0];
    }

    tick = (tick + 1) % 60;
    
    // Apply Action
    if (env.actions[0] >= 0 && env.actions[0] <= 6) {
        if (gameMode == 1) {
            // Player vs AI: Split Step
            c_step_player(&env);
            CheckGameOver();
            
            if (!gameOver) {
                // Schedule AI turn
                aiTurnPending = true;
                aiDelayTimer = 0.0f;
            }
        } else {
            // AI vs AI (or manual override): Atomic Step
            c_step(&env);
            CheckGameOver();
        }
    }
}

// Helper to draw winning lines
void DrawWinningLine(CConnect4* env, int winner) {
    uint64_t pieces = (winner == 1) ? env->player_pieces : env->env_pieces;
    
    // Re-calculate board position
    int board_height = ROWS * PIECE_HEIGHT;
    int board_width = COLUMNS * PIECE_WIDTH;
    int start_y = (GetScreenHeight() - board_height) / 2 + 50; 
    int start_x = (GetScreenWidth() - board_width) / 2;
    
    // Check Horizontal
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c <= COLUMNS - 4; c++) {
            int idx = c * (ROWS + 1) + r;
            if (((pieces >> idx) & 1) && ((pieces >> (idx + 7)) & 1) &&
                ((pieces >> (idx + 14)) & 1) && ((pieces >> (idx + 21)) & 1)) {
                
                // Draw Line
                // r=0 is bottom (index logic), but screen Y is inverted: start_y + (ROWS-1-r)*PH
                float y = start_y + (ROWS - 1 - r) * PIECE_HEIGHT + PIECE_HEIGHT/2.0f;
                float x1 = start_x + c * PIECE_WIDTH + PIECE_WIDTH/2.0f;
                float x2 = start_x + (c+3) * PIECE_WIDTH + PIECE_WIDTH/2.0f;
                
                DrawLineEx((Vector2){x1, y}, (Vector2){x2, y}, 10, GREEN);
            }
        }
    }

    // Check Vertical
    for (int c = 0; c < COLUMNS; c++) {
        for (int r = 0; r <= ROWS - 4; r++) {
            int idx = c * (ROWS + 1) + r;
            if (((pieces >> idx) & 1) && ((pieces >> (idx + 1)) & 1) &&
                ((pieces >> (idx + 2)) & 1) && ((pieces >> (idx + 3)) & 1)) {
                
                float x = start_x + c * PIECE_WIDTH + PIECE_WIDTH/2.0f;
                float y1 = start_y + (ROWS - 1 - r) * PIECE_HEIGHT + PIECE_HEIGHT/2.0f;
                float y2 = start_y + (ROWS - 1 - (r+3)) * PIECE_HEIGHT + PIECE_HEIGHT/2.0f;
                
                DrawLineEx((Vector2){x, y1}, (Vector2){x, y2}, 10, GREEN);
            }
        }
    }

    // Check Diagonal / (Up-Right)
    for (int c = 0; c <= COLUMNS - 4; c++) {
        for (int r = 0; r <= ROWS - 4; r++) {
            int idx = c * (ROWS + 1) + r;
            // Check (c,r), (c+1,r+1), (c+2,r+2), (c+3,r+3)
            if (((pieces >> idx) & 1) && 
                ((pieces >> (idx + (ROWS + 2))) & 1) &&
                ((pieces >> (idx + 2*(ROWS + 2))) & 1) && 
                ((pieces >> (idx + 3*(ROWS + 2))) & 1)) {
                
                float x1 = start_x + c * PIECE_WIDTH + PIECE_WIDTH/2.0f;
                float y1 = start_y + (ROWS - 1 - r) * PIECE_HEIGHT + PIECE_HEIGHT/2.0f;
                float x2 = start_x + (c+3) * PIECE_WIDTH + PIECE_WIDTH/2.0f;
                float y2 = start_y + (ROWS - 1 - (r+3)) * PIECE_HEIGHT + PIECE_HEIGHT/2.0f;
                DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 10, GREEN);
            }
        }
    }

    // Check Diagonal \ (Down-Right)
    for (int c = 0; c <= COLUMNS - 4; c++) {
        for (int r = 3; r < ROWS; r++) {
            int idx = c * (ROWS + 1) + r;
            if (((pieces >> idx) & 1) && 
                ((pieces >> (idx + ROWS)) & 1) &&
                ((pieces >> (idx + 2*ROWS)) & 1) && 
                ((pieces >> (idx + 3*ROWS)) & 1)) {
                
                float x1 = start_x + c * PIECE_WIDTH + PIECE_WIDTH/2.0f;
                float y1 = start_y + (ROWS - 1 - r) * PIECE_HEIGHT + PIECE_HEIGHT/2.0f;
                float x2 = start_x + (c+3) * PIECE_WIDTH + PIECE_WIDTH/2.0f;
                float y2 = start_y + (ROWS - 1 - (r-3)) * PIECE_HEIGHT + PIECE_HEIGHT/2.0f;
                DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 10, GREEN);
            }
        }
    }
}

void DrawConnect4() {
    if (!initialized) return;
    c_render(&env);
    
    // Draw Top UI Buttons
    Rectangle modeBtn = { 10, 10, 200, 40 };
    DrawRectangleRec(modeBtn, LIGHTGRAY);
    DrawRectangleLinesEx(modeBtn, 2, DARKGRAY);
    const char* modeText = (gameMode == 0) ? "Mode: AI vs AI" : "Mode: Player vs AI";
    int textW = MeasureText(modeText, 20);
    DrawText(modeText, modeBtn.x + modeBtn.width/2 - textW/2, modeBtn.y + 10, 20, BLACK);
    
    Rectangle resetBtn = { 220, 10, 100, 40 };
    DrawRectangleRec(resetBtn, LIGHTGRAY);
    DrawRectangleLinesEx(resetBtn, 2, DARKGRAY);
    
    // Label changes if game over
    const char* resetText = gameOver ? "New Game" : "Reset";
    // Measure text to center it in button (approx)
    // "Reset" is shorter than "New Game", centering logic:
    int resetTextW = MeasureText(resetText, 20);
    DrawText(resetText, resetBtn.x + (resetBtn.width - resetTextW)/2, resetBtn.y + 10, 20, BLACK);
    
    // Draw Winning Line if game over
    if (gameOver && winner != 0 && winner != 2) {
        DrawWinningLine(&env, winner);
    }
    
    // Status Text Area (Above board)
    if (gameOver) {
        const char* msg = "GAME OVER";
        Color msgColor = WHITE;
        if (winner == 1) { 
            if (gameMode == 0) msg = "BLUE AI WINS!";
            else msg = "PLAYER WINS!"; 
            msgColor = BLUE; 
        }
        else if (winner == -1) { 
            if (gameMode == 0) msg = "RED AI WINS!";
            else msg = "AI WINS!"; 
            msgColor = RED; 
        }
        else if (winner == 2) { msg = "DRAW!"; msgColor = GRAY; }
        
        int statusTextW = MeasureText(msg, 30);
        DrawText(msg, GetScreenWidth()/2 - statusTextW/2, 70, 30, msgColor);
        
    } else if (aiTurnPending) {
        DrawText("AI is thinking...", 10, 70, 20, WHITE);
    }
}

void UnloadConnect4() {
    if (!initialized) return;
    
    if (net) {
        free_linearlstm(net);
        net = NULL;
    }
    if (weights) {
        free(weights);
        weights = NULL;
    }
    if (env.client) {
        close_client(env.client);
        env.client = NULL;
    }
    free_allocated_cconnect4(&env);
    
    initialized = false;
}
