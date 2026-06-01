/*
 * Game "Kosoban" for 8 bit computers
 * Programming (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 * Levels and idea (c) Andrey Shevchuk campugnatus@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <c8080/hal.h>
#include <c8080/delay.h>
#include <c8080/keys.h>
#include <c8080/colors.h>
#include <c8080/remainder.h>
#include <c8080/console.h>
#include "levels.h"
#include "resources.h"
#include "consts.h"

static const uint8_t MAP_WIDTH = 14;
static const uint8_t MAP_HEIGHT = 7;

static uint8_t mapStatic[MAP_HEIGHT][MAP_WIDTH];
static uint8_t mapBox[MAP_HEIGHT][MAP_WIDTH];
static uint8_t level;
static uint8_t cursorX, cursorY;
static uint8_t cursorPrevX, cursorPrevY;
static uint8_t imageColor;

static bool SelectNextBox(void);

static void *GetCellSprite(uint8_t x, uint8_t y) {
    const uint8_t boxItem = mapBox[y][x];
    const uint8_t staticItem = mapStatic[y][x];
    uint8_t sprite;
    if (boxItem != EMPTY) {
        sprite = 1 + (boxItem >> DIR_SHIFT);  // block
        imageColor = GET_COLOR(boxItem);
        if (staticItem == imageColor)
            sprite += 4;  // block + done
        imageColor = BOX_COLOR(imageColor);
    } else if (staticItem == EMPTY) {
        imageColor = COLOR_INK_WHITE;
        sprite = 0;  // empty cell
    } else if (staticItem < DIR_UP) {
        imageColor = DEST_COLOR(staticItem);
        sprite = 1;  // destination
    } else {
        imageColor = COLOR_INK_WHITE;
        sprite = 9 + (staticItem >> DIR_SHIFT);  // box + direction
    }
    if (cursorX == x && cursorY == y)
        sprite += 15;  // + cursor rect
    return imgCell[sprite];
}

static void DrawCellAny(uint8_t x, uint8_t y, const void *image, uint8_t color) {
    DrawImageColorTileXY(GAME_X + x * CELL_W, GAME_Y + y * CELL_H, color, image);
}

static void DrawCell(uint8_t x, uint8_t y) {
    const void *image = GetCellSprite(x, y);
    DrawCellAny(x, y, image, imageColor);
}

static void DrawCursor(void) {
    DrawCell(cursorPrevX, cursorPrevY);
    DrawCell(cursorX, cursorY);
    cursorPrevX = cursorX;
    cursorPrevY = cursorY;
}

static void DrawLevelNumber(void) {
    char buf[16];
    snprintf(buf, sizeof(buf), "Уровень %2u", (uint16_t)level);
    DrawTextXY(LEVEL_X, LEVEL_Y, LEVEL_COLOR, buf);
}

static void LevelAnimation1(bool mode) {
    uint8_t i = 0;
    do {
        i = i * 5 + 1;
        if (i < MAP_WIDTH * MAP_HEIGHT) {
            const uint8_t y = i / MAP_WIDTH;
            const uint8_t x = __remainder;  // TODO: div
            if (mode) {
                DrawCell(x, y);
            } else {
                uint8_t c = rand() & 7;
                if (c == 0)
                    c = 7;
                DrawCellAny(x, y, imgCell[14], BOX_COLOR(c));
            }
            DELAY_MS(5);
        }
    } while (i != 0);
}

static void LevelAnimation(void) {
    LevelAnimation1(false);
    LevelAnimation1(true);
    cursorPrevX = cursorX;
    cursorPrevY = cursorY;
}

static void MoveAnimation(uint8_t dx, uint8_t dy, uint8_t count) {
    const uint8_t x = cursorX - dx;
    const uint8_t y = cursorY - dy;
    uint8_t gx = GAME_X + x * CELL_W;
    uint8_t gy = GAME_Y + y * CELL_H;
    uint8_t i = 0;
    for (;;) {
        uint8_t x1 = cursorX;
        uint8_t y1 = cursorY;
        gx += dx;
        gy += dy * MOVE_ANIMATION_H;
        uint8_t gx1 = gx;
        uint8_t gy1 = gy;
        DrawCell(x, y);
        for (uint8_t j = 0; j < count; j++) {
            void *image = GetCellSprite(x1, y1);
            DrawImageColorTileXY(gx1, gy1, imageColor, image);
            gx1 += dx * CELL_W;
            gy1 += dy * CELL_H;
            x1 += dx;
            y1 += dy;
        }

        if (dy == 0) {
            if (i == CELL_W - 1)
                break;
        } else {
            if (i == CELL_H / MOVE_ANIMATION_H - 1)
                break;
        }

        i++;
        DELAY_MS(100);
    }
    cursorPrevX = cursorX;
    cursorPrevY = cursorY;
}

static bool WinAnimation1(void) {
    for (uint8_t i = 0; i < 20; i++) {
        if (!SelectNextBox())
            return false;
        DELAY_MS(50);
    }
    return true;
}

static void WinAnimation2(void) {
    for (uint8_t i = 0; i < 20; i++) {
        cursorX ^= 0xFF;
        DrawCell(cursorPrevX, cursorPrevY);
        DELAY_MS(50);
    }
}

static void WinAnimation(void) {
    const uint8_t x = cursorX;
    const uint8_t y = cursorY;
    if (!WinAnimation1())
        WinAnimation2();
    cursorX = x;
    cursorY = y;
}

static void Intro(void) {
    DrawScreen(imgIntro);

    static const uint8_t INTRO_WIDTH = 64;

    static const char text[] =
        "Программирование © Алексей Морозов (aleksey.f.morozov@gmail.com)"
        "   Идея игры и уровни © Андрей Шевчук (campugnatus@gmail.com)   "
        "                 Изображение девушки © ZEUS II                  "
        "                         12 августа 2025                        ";

    char line[INTRO_WIDTH + 1];
    memset(line, ' ', INTRO_WIDTH);
    line[INTRO_WIDTH] = 0;

    const char *i = text;
    uint8_t n = 0;
    for (;;) {
        memmove(line, line + 1, INTRO_WIDTH - 1);
        if (*i == 0)
            i = text;
        line[INTRO_WIDTH - 1] = *i;
        i++;

        DrawTextXY(0, INTRO_Y, INTRO_COLOR, line);

        uint8_t delay = 1;
        if (++n == INTRO_WIDTH) {
            n = 0;
            delay = 40;
        }

        do {
            DELAY_MS(50);
            if (InKey() != 0xFF)
                return;
        } while (delay--);
    }
}

static void LoadLevel(void) {
    DrawLevelNumber();

    memset(mapStatic, EMPTY, sizeof(mapStatic));
    memset(mapBox, EMPTY, sizeof(mapBox));

    const uint8_t *src = levels + (level - 1) * LEVEL_SIZE;

    uint8_t *dest = &mapStatic[(MAP_HEIGHT - LEVEL_HEIGHT + 1) / 2][(MAP_WIDTH - LEVEL_WIDTH + 1) / 2];
    for (uint8_t y = 0; y < LEVEL_HEIGHT; y++) {
        memcpy(dest, src, LEVEL_WIDTH);
        src += LEVEL_WIDTH;
        dest += MAP_WIDTH;
    }

    dest = &mapBox[(MAP_HEIGHT - LEVEL_HEIGHT + 1) / 2][(MAP_WIDTH - LEVEL_WIDTH + 1) / 2];
    for (uint8_t y = 0; y < LEVEL_HEIGHT; y++) {
        memcpy(dest, src, LEVEL_WIDTH);
        src += LEVEL_WIDTH;
        dest += MAP_WIDTH;
    }

    LevelAnimation();
}

static void LoadNextLevel(void) {
    level++;
    if (level > LEVEL_COUNT)
        level = 1;

    LoadLevel();
}

static bool SelectNextBox(void) {
    do {
        cursorX++;
        if (cursorX == MAP_WIDTH) {
            cursorY++;
            if (cursorY == MAP_HEIGHT)
                cursorY = 0;
            cursorX = 0;
        }
    } while (mapBox[cursorY][cursorX] == EMPTY);

    if (cursorX == cursorPrevX && cursorY == cursorPrevY)
        return false;

    DrawCursor();
    return true;
}

static void CheckLevelDone(void) {
    uint8_t *staticI = mapStatic[0];
    uint8_t *boxI = mapBox[0];
    bool done = true;
    for (uint8_t y = 0; y < MAP_HEIGHT; y++) {
        for (uint8_t x = 0; x < MAP_WIDTH; x++) {
            const uint8_t staticItem = *staticI;
            if (staticItem != EMPTY) {
                if (staticItem < DIR_UP) {
                    if (staticItem != GET_COLOR(*boxI))
                        done = false;
                } else if (*boxI != EMPTY) {  // Изменение направления
                    *boxI = GET_COLOR(*boxI) | GET_DIR(staticItem);
                    DrawCell(x, y);
                }
            }
            staticI++;
            boxI++;
        }
    }
    if (done) {
        WinAnimation();
        LoadNextLevel();
    }
}

static void MoveBox(int8_t dx, int8_t dy) {
    // Подсчет кол-ва толкаемых коробок
    uint8_t count = 0;
    uint8_t x = cursorX;
    uint8_t y = cursorY;
    while (mapBox[y][x] != EMPTY) {
        count++;
        x += dx;
        y += dy;
        if (x >= MAP_WIDTH || y >= MAP_HEIGHT)
            return;
    }

    // Нечего толкать
    if (count == 0)
        return;

    // Толкаем
    for (uint8_t i = count; i != 0; i--) {
        mapBox[y][x] = mapBox[y - dy][x - dx];
        x -= dx;
        y -= dy;
    }
    mapBox[y][x] = EMPTY;

    // Перемещаем курсор
    cursorX += dx;
    cursorY += dy;

    // Анимация
    MoveAnimation(dx, dy, count);

    // Поворот ящиков и проверка конца уровня.
    CheckLevelDone();
}

static void Click(void) {
    switch (GET_DIR(mapBox[cursorY][cursorX])) {
        case DIR_UP:
            MoveBox(0, -1);
            break;
        case DIR_RIGHT:
            MoveBox(1, 0);
            break;
        case DIR_DOWN:
            MoveBox(0, 1);
            break;
        case DIR_LEFT:
            MoveBox(-1, 0);
            break;
    }
}

int main(int, char **) {
    Intro();

    level = 1;
    cursorPrevX = cursorX = MAP_WIDTH / 2;
    cursorPrevY = cursorY = MAP_HEIGHT / 2;

    DrawScreen(imgGame);
    LoadLevel();

    for (;;) {
        switch (ReadAndDecodeConsoleKeys()) {
            case '1':
                LoadLevel();
                break;
            case '3':
                LoadNextLevel();
                break;
            case KEY_LEFT:
                cursorX--;
                if (cursorX >= MAP_WIDTH)
                    cursorX = MAP_WIDTH - 1;
                DrawCursor();
                break;
            case KEY_RIGHT:
                cursorX++;
                if (cursorX >= MAP_WIDTH)
                    cursorX = 0;
                DrawCursor();
                break;
            case KEY_UP:
                cursorY--;
                if (cursorY >= MAP_HEIGHT)
                    cursorY = MAP_HEIGHT - 1;
                DrawCursor();
                break;
            case KEY_DOWN:
                cursorY++;
                if (cursorY >= MAP_HEIGHT)
                    cursorY = 0;
                DrawCursor();
                break;
            case KEY_ENTER:
                SelectNextBox();
                break;
            case KEY_SPACE:
                Click();
                break;
        }
    }
    return 0;
}
