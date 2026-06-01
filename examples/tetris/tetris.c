// Simple tetris
// MIT License
// Copyright (c) 2024 svedev
// https://github.com/svedev0/tetris-c

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <c8080/keys.h>
#include <c8080/console.h>

#include "tetris.h"

static const unsigned GAME_SPEED = 3000;

const int tetrominoes[7][16] = {
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},  // I
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // O
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},  // S
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // Z
    {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // T
    {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // L
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0}   // J
};

int arena[A_HEIGHT][A_WIDTH];

uint32_t score = 0;
bool gameOver = false;
int currTetrominoIdx;
int currRotation = 0;
int currX = A_WIDTH / 2;
int currY = 0;

int main(int, char **) {
    HideCursor();
    ClearConsole();
    memset(arena, 0, sizeof(arena[0][0]) * A_HEIGHT * A_WIDTH);
    newTetromino();

    unsigned elapsed = 0;

    while (!gameOver) {
        processInputs();

        if (elapsed > 0) {
            elapsed--;
            continue;
        }
        elapsed = GAME_SPEED;

        if (!moveDown()) {
            addToArena();
            checkLines();
            newTetromino();
        }

        drawArena();
    }

    ClearConsole();
    printf("Game over!\nScore: %d\n", score);
    ShowCursor();
    return 0;
}

void newTetromino() {
    currTetrominoIdx = rand() % 7U;
    currRotation = 0;
    currX = (A_WIDTH / 2) - (T_WIDTH / 2);
    currY = 0;
    gameOver = !validPos(currTetrominoIdx, currRotation, currX, currY);
}

bool validPos(int tetromino, int rotation, int posX, int posY) {
    for (int x = 0; x < T_WIDTH; x++) {
        for (int y = 0; y < T_HEIGHT; y++) {
            int index = rotate(x, y, rotation);
            if (1 != tetrominoes[tetromino][index]) {
                continue;
            }

            int arenaX = x + posX;
            int arenaY = y + posY;
            if (0 > arenaX || A_WIDTH <= arenaX || A_HEIGHT <= arenaY) {
                return false;
            }

            int arenaXY = arena[arenaY][arenaX];
            if (0 <= arenaY && 1 == arenaXY) {
                return false;
            }
        }
    }
    return true;
}

int rotate(int x, int y, int rotation) {
    switch (rotation % 4) {
        case 0:
            return x + y * T_WIDTH;
        case 1:
            return 12 + y - (x * T_WIDTH);
        case 2:
            return 15 - (y * T_WIDTH) - x;
        case 3:
            return 3 - y + (x * T_WIDTH);
        default:
            return 0;
    }
}

void processInputs() {
    if (!CanReadConsole()) {
        return;
    }

    while (CanReadConsole()) {
        int key = ReadAndDecodeConsoleKeys();
        switch (key) {
            case KEY_UP:  // Up arrow key
                int nextRotation = (currRotation + 1) % 4;
                if (validPos(currTetrominoIdx, nextRotation, currX, currY)) {
                    currRotation = nextRotation;
                }
                drawArena();
                break;
            case KEY_LEFT:  // Left arrow key
                if (validPos(currTetrominoIdx, currRotation, currX - 1, currY)) {
                    currX--;
                }
                drawArena();
                break;
            case KEY_RIGHT:  // Right arrow key
                if (validPos(currTetrominoIdx, currRotation, currX + 1, currY)) {
                    currX++;
                }
                drawArena();
                break;
            case KEY_DOWN:  // Down arrow key
                while (validPos(currTetrominoIdx, currRotation, currX, currY + 1)) {
                    currY++;
                }
                drawArena();
                break;
        }
    }
}

bool moveDown() {
    if (validPos(currTetrominoIdx, currRotation, currX, currY + 1)) {
        currY++;
        return true;
    }
    return false;
}

void addToArena() {
    for (int y = 0; y < T_HEIGHT; y++) {
        for (int x = 0; x < T_WIDTH; x++) {
            int index = rotate(x, y, currRotation);
            if (1 != tetrominoes[currTetrominoIdx][index]) {
                continue;
            }

            int arenaX = currX + x;
            int arenaY = currY + y;
            bool xInRange = (0 <= arenaX) && (arenaX < A_WIDTH);
            bool yInRange = (0 <= arenaY) && (arenaY < A_HEIGHT);
            if (xInRange && yInRange) {
                arena[arenaY][arenaX] = 1;
            }
        }
    }
}

void checkLines() {
    int clearedLines = 0;

    for (int y = A_HEIGHT - 1; y >= 0; y--) {
        bool lineFull = true;
        for (int x = 0; x < A_WIDTH; x++) {
            if (0 == arena[y][x]) {
                lineFull = false;
                break;
            }
        }

        if (!lineFull) {
            continue;
        }

        clearedLines++;
        for (int yy = y; yy > 0; yy--) {
            for (int xx = 0; xx < A_WIDTH; xx++) {
                arena[yy][xx] = arena[yy - 1][xx];
            }
        }

        for (int xx = 0; xx < A_WIDTH; xx++) {
            arena[0][xx] = 0;
        }
        y++;
    }

    if (0 < clearedLines) {
        score += 100 * clearedLines;
    }
}

void drawArena() {
    MoveCursorHome();

    char buffer[512];
    int bufferIndex = 0;

    for (int y = 0; y < A_HEIGHT; y++) {
        buffer[bufferIndex++] = '|';

        for (int x = 0; x < A_WIDTH; x++) {
            int rotatedPos = rotate(x - currX, y - currY, currRotation);
            bool validX = x >= currX && x < currX + T_WIDTH;
            bool validY = y >= currY && y < currY + T_HEIGHT;
            bool xyFilled = 1 == tetrominoes[currTetrominoIdx][rotatedPos];

            if (1 == arena[y][x] || (validX && validY && xyFilled)) {
                buffer[bufferIndex++] = '#';
            } else {
                buffer[bufferIndex++] = ' ';
            }
        }

        buffer[bufferIndex++] = '|';
        buffer[bufferIndex++] = '\n';
    }

    buffer[bufferIndex] = '\0';
    printf("%s\n\nScore: %d\n\n", buffer, score);
}
