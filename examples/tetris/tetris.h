// Simple tetris
// MIT License
// Copyright (c) 2024 svedev
// https://github.com/svedev0/tetris-c

#define A_WIDTH 10   // Arena width
#define A_HEIGHT 20  // Arena height
#define T_WIDTH 4    // Tetromino width
#define T_HEIGHT 4   // Tetromino height

// Generate a new tetromino
void newTetromino();

// Determine if a position is valid
bool validPos(int tetromino, int rotation, int posX, int posY);

// Rotate tetromino
int rotate(int x, int y, int rotation);

// Process keyboard inputs
void processInputs();

// Move tetromino down
bool moveDown();

// Add tetromino to arena
void addToArena();

// Check if lines can be cleared
void checkLines();

// Draw arena
void drawArena();
