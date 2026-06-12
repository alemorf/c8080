/*
 * Game "Color Lines" for 8 bit computers
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <c8080/hal.h>
#include <c8080/keys.h>
#include <c8080/delay.h>
#include <c8080/remainder.h>
#include <c8080/uint16tostring.h>
#include "lines.h"
#include "path.h"
#include "music.h"
#include "resources.h"
#include "consts.h"
#include <unistd.h>

static const uint8_t REMOVE_ANIMATION_COUNT = 3; /* phase для DrawSpriteRemove */
static const uint8_t NEW_ANIMATION_COUNT = 5;    /* phase для DrawSpriteNew */
static const uint8_t BOUNCE_ANIMATION_COUNT = 6; /* phase для DrawBouncingBall */
static const uint16_t BOUNCE_ANIMATION_DELAY = 50;

static const uint8_t LAST_STEP = 9;
static const uint8_t HISCORE_COUNT = 8;
static const uint8_t COLORS_COUNT = 7;
static const uint8_t NEW_BALL_COUNT = 3;
static const uint8_t NO_SEL = 0xFF;  // Значение для selX

uint8_t game[GAME_WIDTH][GAME_HEIGHT];
uint8_t cursorX, cursorY;
uint8_t selX, selY;
uint16_t score;
uint8_t newBalls[NEW_BALL_COUNT];
bool showPath = true;
bool soundEnabled = true;
bool showHelp = true;
uint8_t selAnimationDelay;
uint8_t selAnimationFrame;
uint8_t playerHeight;

struct HiScore {
    char name[10];
    uint16_t score;
};

struct HiScore hiScores[] = {
    {"Alemorf", 300}, {"B2M", 250}, {"Eltaron", 200}, {"Error404", 150},
    {"SYSCAT", 100},  {"Mick", 80}, {"Svofski", 60},  {"Titus", 40},
};

void *CellAddress(uint8_t x, uint8_t y) {
    x = PLAYFIELD_X + x * CELL_TILE_WIDTH;
#ifdef CELL_HALF_HEIGHT
    y = PLAYFIELD_Y + (unsigned)(y * (CELL_TILE_HEIGHT * 2 - 1) + 1) / 2;
#else
    y = PLAYFIELD_Y + y * CELL_TILE_HEIGHT;
#endif
    return TILE(x, y);
}

void DrawBall(uint8_t *tile, uint8_t y, uint8_t sprite, uint8_t color) {
    color = BALL_COLOR(color);
#ifdef CELL_HALF_HEIGHT
    if (y & 1) {
        DrawImageColorTile(tile, color, imgBalls1[sprite]);
        return;
    }
#endif
    DrawImageColorTile(tile, color, imgBalls[sprite]);
}

static void DrawBallXY(uint8_t x, uint8_t y, uint8_t sprite, uint8_t color) {
    DrawBall(CellAddress(x, y), y, sprite, color);
}

void DrawCursor(void) {
    uint8_t c = game[cursorX][cursorY];
    DrawBallXY(cursorX, cursorY, c ? 2 : 1, c);
}

// Удаляем линни из 5 шариков и больше

static void ClearLine(uint8_t x0, uint8_t y0, uint8_t dx, uint8_t dy, uint8_t length) {
    register uint8_t x, y, o, i;

    // Анимация исчезновения шариков
    const int8_t color = game[x0][y0];
    for (o = 0; o < REMOVE_ANIMATION_COUNT; o++) {
        x = x0;
        y = y0;
        for (i = length; i != 0; --i) {
            DrawBallXY(x, y, 12 + o, color);
            x += dx;
            y += dy;
        }
        DELAY_MS(100);
    }

    // Очищаем экран и массив
    x = x0;
    y = y0;
    for (i = length; i != 0; --i) {
        game[x][y] = 0;
        DrawBallXY(x, y, 0, 0);
        x += dx;
        y += dy;
    }
}

static char *Center(char *buffer, uint16_t value) {
    char *text = Uint16ToString(buffer, value, 10);
#ifdef CENTER_SCORE
    buffer += (UINT16_TO_STRING_SIZE - 1 - strlen(text)) / 2;
#endif
    return buffer;
}

static void DrawScoreAndCreatures2(void) {
    char buffer[UINT16_TO_STRING_SIZE];
    DrawText(DRAWTEXTARGS(SCORE_X, SCORE_Y), SCORE_COLOR, Center(buffer, score));

    uint16_t n;
    n = (score / (hiScores[0].score / PLAYAR_MAX_HEIGHT));
    if (n > PLAYAR_MAX_HEIGHT)
        n = PLAYAR_MAX_HEIGHT;

    if (playerHeight != n) {
        playerHeight = n;
        uint8_t y = PLAYER_Y - imgPlayerDHeight;
        for (; n > 0; n--) {
            DrawImageTileXY(PLAYER_X, y, imgPlayerD);
            y -= imgPlayerDHeight;
        }
        y -= (imgPlayerHeight - imgPlayerDHeight);
        DrawImageTileXY(PLAYER_X, y, imgPlayer);
        if (playerHeight == PLAYAR_MAX_HEIGHT) {
            DrawImageTileXY(PLAYER_X + PLAYER_CROWN_DX, y - PLAYER_CROWN_DY, imgPlayerWin);
            DrawImageTile(TILE(KING_X, KING_Y), imgKingLose);
        }
    }
}

// Ищем линни из 5 шариков и больше

static uint8_t FindLines(void) {
    register uint8_t x, y;
    uint8_t prevx, prevy;
    uint8_t c, n, total = 0;
    uint8_t *p;

    for (y = 0; y != GAME_HEIGHT; y++) {
        for (p = &game[0][y], x = 0; x < GAME_WIDTH;) {
            prevx = x;
            c = *p;
            ++x;
            p += GAME_WIDTH;
            if (c == 0)
                continue;
            while (x != GAME_WIDTH && c == *p) {
                p += GAME_WIDTH;
                ++x;
            }
            n = x - prevx;
            if (n < 5)
                continue;
            ClearLine(prevx, y, 1, 0, n);
            total += n;
            break;
        }
    }
    for (x = 0; x != GAME_WIDTH; ++x) {
        for (p = &game[x][0], y = 0; y < 5;) {
            c = *p;
            prevy = y;
            ++y;
            ++p;
            if (c == 0)
                continue;
            while (y != GAME_HEIGHT && c == *p) {
                ++p;
                ++y;
            }
            n = y - prevy;
            if (n < 5)
                continue;
            ClearLine(x, prevy, 0, 1, n);
            total += n;
            break;
        }
    }
    for (y = 0; y != 6; ++y) {
        for (x = 0; x != 6; ++x) {
            p = &game[x][y];
            c = *p;
            if (c == 0)
                continue;
            prevx = x;
            prevy = y;
            while (1) {
                ++prevy;
                ++prevx;
                p += GAME_WIDTH + 1;
                if (prevx == GAME_WIDTH)
                    break;
                if (prevy == GAME_HEIGHT)
                    break;
                if (c != *p)
                    break;
            }
            n = prevy - y;
            if (n < 5)
                continue;
            ClearLine(x, y, 1, 1, n);
            total += n;
        }
    }
    for (y = 0; y != 6; ++y) {
        for (x = 4; x != GAME_WIDTH; ++x) {
            p = &game[x][y];
            c = *p;
            if (c == 0)
                continue;
            prevx = x;
            prevy = y;
            while (1) {
                ++prevy;
                --prevx;
                p += 1 - GAME_WIDTH;
                if (prevx == -1)
                    break;
                if (prevy == GAME_HEIGHT)
                    break;
                if (c != *p)
                    break;
            }
            n = prevy - y;
            if (n < 5)
                continue;
            ClearLine(x, y, -1, 1, n);
            total += n;
        }
    }
    if (total == 0)
        return 0;

    // Результат был изменен, перерисуем его
    score += total * 2;
    // TODO Overflow
    DrawScoreAndCreatures2();
    return 1;
}

// Посчитать кол-во свободных клеток

static uint8_t CalcFreeCellCount(void) {
    uint8_t *cell = &game[0][0];
    uint8_t freeCellCount = 0;
    uint8_t i;
    for (i = GAME_WIDTH * GAME_HEIGHT; i != 0; --i) {
        if (*cell == 0)
            freeCellCount++;
        cell++;
    }
    return freeCellCount;
}

// Вычисляем цвета новых шариков

static void GenerateNewBalls(void) {
    uint8_t i;
    for (i = 0; i < NEW_BALL_COUNT; i++)
        newBalls[i] = (uint8_t)rand() % COLORS_COUNT + 1;
}

static void DrawHelp(void) {
    DrawBall(TILE(HELP_1_X, HELP_1_Y), 0, 18, newBalls[0]);
    DrawBall(TILE(HELP_2_X, HELP_2_Y), 0, 18, newBalls[1]);
    DrawBall(TILE(HELP_3_X, HELP_3_Y), 0, 18, newBalls[2]);
}

static void HideHelp(void) {
    DrawBall(TILE(HELP_1_X, HELP_1_Y), 0, 19, 0);
    DrawBall(TILE(HELP_2_X, HELP_2_Y), 0, 19, 0);
    DrawBall(TILE(HELP_3_X, HELP_3_Y), 0, 19, 0);
}

// Показываем цвета новых шариков

void DrawHelp2(void) {
    if (showHelp) {
        DrawHelp();
        return;
    }
    HideHelp();
}

// Помещаем шарик в случайную свободную ячейку

struct XY {
    uint8_t x, y;
};

// Удалить линии или добавить 3 шарика.

static uint8_t GameStep(uint8_t newGame) {
    // Ищем готовые линии
    if (FindLines())
        return 0;

    // Считаем кол во свободных клеток
    uint8_t freeCellCount = CalcFreeCellCount();
    if (freeCellCount == 0)
        return 1;  // Такого не может быть

    // Кол-во добавляемых шариков
    uint8_t newBallCount = freeCellCount;
    if (newBallCount > NEW_BALL_COUNT)
        newBallCount = NEW_BALL_COUNT;

    // Вычисляем координаты новых шариков
    uint8_t i;
    struct XY coords[NEW_BALL_COUNT];
    for (i = 0; i < newBallCount; i++) {
        uint8_t n = (unsigned)rand() % freeCellCount;
        freeCellCount--;
        uint8_t *p = &game[0][0];
        for (;;) {
            if (*p == 0) {
                if (n == 0)
                    break;
                n--;
            }
            p++;
        }

        // Заносим шарик
        *p = newBalls[i];

        // Запоминаем координаты для анимации
        coords[i].x = (p - &game[0][0]) / GAME_WIDTH;
        coords[i].y = __remainder;
    }

    // Рисуем анимацию
    if (!newGame) {
        for (i = 0; i < NEW_ANIMATION_COUNT; i++) {
            uint8_t j;
            for (j = 0; j < newBallCount; j++)
                DrawBallXY(coords[j].x, coords[j].y, 7 + i, newBalls[j]);
            DELAY_MS(50);
        }
    }

    // Вычисляем цвета новых шариков
    GenerateNewBalls();

    // Рисуем цвета новых шариков
    if (newGame)
        return 0;
    if (showHelp)
        DrawHelp2();

    // Ищем готовые линии
    if (FindLines())
        return 0;

    // Если свободных клеток не осталось и целых линий не обнаружено, то конец игры
    return freeCellCount == 0;
}

static void DrawHiScoresItem(uint8_t y, uint8_t color, const char *text) {
    DrawTextXY(HI_X, HI_Y + y * HI_LINE_HEIGHT, color, text);
}

static void DrawHiScoresScreen(uint8_t i, uint8_t hilightPos) {
    struct HiScore *h = hiScores + i;
    for (; i < HISCORE_COUNT; ++i) {
        char text[10 + 1 + 5 + 1];
        h->name[9] = '\0';
        snprintf(text, sizeof(text), "%10s %5u", h->name, h->score);
        DrawHiScoresItem(i + 2, hilightPos == i ? HI_COLOR_1 : HI_COLOR_0, text);
        h++;
    }
}

static void DrawHiScoresWindow2(uint8_t hilight, const char *lastLine) {
    uint8_t y;
    uint8_t gy = HIRECT_Y;
    for (y = 0; y < HIRECT_HEIGHT; y++) {
        uint8_t gx = HIRECT_X;
        DrawImageTileXY(gx, gy, imgHi[0]);
        gx += imgHiWidth;
        uint8_t x;
        for (x = 0; x < HIRECT_WIDTH; x++) {
            DrawImageTileXY(gx, gy, imgHi[1]);
            gx += imgHiWidth;
        }
        DrawImageTileXY(gx, gy, imgHi[2]);
        gy += CELL_TILE_HEIGHT;
    }
    DrawHiScoresItem(0, HI_COLOR_3, "    Рекорды");
    DrawHiScoresScreen(0, hilight);
    DrawHiScoresItem(HISCORE_COUNT + 3, HI_COLOR_2, lastLine);
}

static void DrawHiScores(void) {
    DrawHiScoresWindow2(HISCORE_COUNT, " Нажмите Пробел ");
}

// Добавить результат в таблицу рекордов

static void AddToHiScores(void) {
    // В нижнюю стрку пользователь будет вводить своё имя
    hiScores[HISCORE_COUNT - 1].score = score;
    hiScores[HISCORE_COUNT - 1].name[0] = 0;

    // Вывод таблицы на экран
    DrawHiScoresWindow2(HISCORE_COUNT - 1, "Введите свое имя");  // TODO: ё

    // Ввод имени
    uint8_t i = 0;
    for (;;) {
        char c = ReadKey();
        if (c == KEY_ENTER)
            break;
        if (c == KEY_BACKSPACE_INKEY) {
            if (i == 0)
                continue;
            --i;
            hiScores[HISCORE_COUNT - 1].name[i] = 0;
            DrawHiScoresScreen(HISCORE_COUNT - 1, HISCORE_COUNT - 1);
            continue;
        }
        if (c < ' ')
            continue;
        if (i == sizeof(hiScores[HISCORE_COUNT - 1].name) - 1)
            continue;
        hiScores[HISCORE_COUNT - 1].name[i] = c;
        ++i;
        hiScores[HISCORE_COUNT - 1].name[i] = 0;
        DrawHiScoresScreen(HISCORE_COUNT - 1, HISCORE_COUNT - 1);
    }

    DrawHiScoresItem(HISCORE_COUNT + 3, HI_COLOR_2, "                ");

    // Анимация перемещения новой позиции вверх
    struct HiScore *p = hiScores + HISCORE_COUNT - 1;
    for (i = HISCORE_COUNT - 1; i != 0; i--) {
        if (p->score < p[-1].score)
            break;
        p--;
        struct HiScore tmp;
        memcpy(&tmp, p + 1, sizeof(tmp));
        memcpy(p + 1, p, sizeof(tmp));
        memcpy(p, &tmp, sizeof(tmp));
        DrawHiScoresScreen(0, i - 1);
        DELAY_MS(100);
    }

    DrawHiScoresItem(HISCORE_COUNT + 3, HI_COLOR_2, " Нажмите Пробел ");
}

static void DrawButtons(void) {
    ChangeTileColor(TILE(BUTTON_PATH_X, BUTTON_PATH_Y), showPath ? BUTTON_COLOR_ENABLED : BUTTON_COLOR_DISABLED,
                    BUTTON_PATH_W, BUTTON_PATH_H);
    ChangeTileColor(TILE(BUTTON_SOUND_X, BUTTON_SOUND_Y), soundEnabled ? BUTTON_COLOR_ENABLED : BUTTON_COLOR_DISABLED,
                    BUTTON_SOUND_W, BUTTON_SOUND_H);
    ChangeTileColor(TILE(BUTTON_HELP_X, BUTTON_HELP_Y), showHelp ? BUTTON_COLOR_ENABLED : BUTTON_COLOR_DISABLED,
                    BUTTON_HELP_W, BUTTON_HELP_H);
}

static void DrawScreen3(void) {
    DrawScreen(imgScreen);

    char buffer[UINT16_TO_STRING_SIZE];
    DrawText(DRAWTEXTARGS(TOPSCORE_X, TOPSCORE_Y), TOPSCORE_COLOR, Center(buffer, hiScores[0].score));

    if (TOPNAME_Y != 0) {
        uint16_t tx = (sizeof(hiScores[0].name) - 1 - strlen(hiScores[0].name)) * 4 / 2 + TOPNAME_X;
        DrawTextXY(tx, TOPNAME_Y, TOPNAME_COLOR, hiScores[0].name);
    }

    playerHeight = -1;  // Redraw
    DrawScoreAndCreatures2();

    DrawButtons();

    uint8_t *a = &game[0][0];
    uint8_t x, y;
    for (x = 0; x < GAME_WIDTH; x++) {
        for (y = 0; y < GAME_HEIGHT; y++) {
            uint8_t c = *a++;
            DrawBallXY(x, y, c ? 11 : 0, c);
        }
    }

    DrawHelp2();
    DrawCursor();
}

// Начать новую игру

static void NewGame(void) {
    cursorX = GAME_WIDTH / 2;
    cursorY = GAME_HEIGHT / 2;
    selX = NO_SEL;
    score = 0;
    memset(game, 0, sizeof(game));

    GenerateNewBalls();
    GameStep(true);
    DrawScreen3();
}

static void PlaySoundCantMove(void) {
    Sound(255, 10);
    DELAY_MS(250);
    Sound(255, 10);
    DELAY_MS(250);
}

static void PlaySoundJump(void) {
    Sound(3, 10);
}

// Переместить шарик

static void MoveBall(void) {
    if (game[cursorX][cursorY] != 0) {
        if (selX != NO_SEL) {
            uint8_t c = game[selX][selY];
            DrawBallXY(selX, selY, c ? 11 : 0, c);
        }
        selX = cursorX;
        selY = cursorY;
        return;
    }

    if (selX == NO_SEL)
        return;

    uint8_t c = game[selX][selY];

    // Алгоритм поиска поути
    if (!PathFind()) {
        if (soundEnabled)
            PlaySoundCantMove();
        return;
    }

    if (showPath) {
        // Рисуем шаги на экране
        for (;;) {
            uint8_t x = path_x;
            uint8_t y = path_y;
            uint8_t dir = PathGetNextStep();
            DrawBallXY(x, y, dir + 2, 0);
            DrawBallXY(path_x, path_y, 11, c);
            if (soundEnabled)
                PlaySoundJump();
            DELAY_MS(100);
            if (path_n == LAST_STEP)
                break;
        };

        // Удаляем нарисованные шаги с экрана
        PathRewind();
        do {
            DrawBallXY(path_x, path_y, 0, 0);
            PathGetNextStep();
        } while (path_n != LAST_STEP);
    } else {
        DrawBallXY(selX, selY, 0, 0);
        DrawBallXY(cursorX, cursorY, 0, c);
    }

    // Очищаем игровое поле от временных значений
    PathFree();

    // Реально перемещаем шарик. Все выше было лишь анимацией.
    game[selX][selY] = 0;
    game[cursorX][cursorY] = c;

    // Снимаем выделение
    selX = NO_SEL;

    // Добавляем 3 шарика
    if (!GameStep(false)) {
        DrawCursor();
        return;
    }

    // Если не получилось добавить, то конец игры.

    // Если игрок набал мало очков, то просто показываем таблицу
    if (score < hiScores[HISCORE_COUNT - 1].score)
        DrawHiScores();
    else
        AddToHiScores();

    ReadKey();
    NewGame();
}

// Анимация прыгающего шарика

static void BouncingBallAnimation(void) {
    if (selX == NO_SEL)
        return;
    selAnimationDelay++;
    if (selAnimationDelay >= BOUNCE_ANIMATION_DELAY) {
        selAnimationDelay = 0;
        static const uint8_t selAnimation[] = {11, 15, 11, 16, 17, 16};
        DrawBallXY(selX, selY, selAnimation[selAnimationFrame], game[selX][selY]);

        selAnimationFrame++;
        if (selAnimationFrame >= BOUNCE_ANIMATION_COUNT) {
            selAnimationFrame = 0;
        } else if (soundEnabled && selAnimationFrame == 3) {
            PlaySoundJump();
        }
    }
}

static void ClearCursor(void) {
    uint8_t c = game[cursorX][cursorY];
    DrawBallXY(cursorX, cursorY, c ? 11 : 0, c);
}

// Главная функция

int main(int, char **) {
    DrawScreen(imgTitle);
    PlayMusic();
    NewGame();

    char previousPressedKey = 0;
    uint8_t keybTimeout = 0;
    for (;;) {
        // Делаем генератор случайных чисел более случайным
        (void)rand();

        // Анимация выбранного шарика
        BouncingBallAnimation();

        char pressedKey = InKey();

        // Устранение дребезга контактов
        if (keybTimeout != 0) {
            keybTimeout--;
            continue;
        }

        // Только факт нажатия
        if (pressedKey == previousPressedKey) {
            Delay(50);
            continue;
        }
        previousPressedKey = pressedKey;
        if (pressedKey == 0)
            continue;
        keybTimeout = 50;  // TODO: magic

        switch (pressedKey) {
#ifdef TEST
            case '6':
                score = (score + 1) * 2;
                DrawScoreAndCreatures2();
                break;
#endif
            case '1':
                showPath = !showPath;
                DrawButtons();
                break;
            case '2':
                soundEnabled = !soundEnabled;
                DrawButtons();
                break;
            case '3':
                showHelp = !showHelp;
                DrawButtons();
                DrawHelp2();
                break;
            case '4':
                DrawHiScores();
                while (ReadKey() != ' ') {
                }
                DrawScreen3();
                break;
            case '5':
                NewGame();
                break;
            case KEY_UP_INKEY:
                ClearCursor();
                if (cursorY == 0)
                    cursorY = GAME_HEIGHT - 1;
                else
                    cursorY--;
                DrawCursor();
                break;
            case KEY_DOWN_INKEY:
                ClearCursor();
                if (cursorY == GAME_HEIGHT - 1)
                    cursorY = 0;
                else
                    cursorY++;
                DrawCursor();
                break;
            case KEY_LEFT_INKEY:
                ClearCursor();
                if (cursorX == 0)
                    cursorX = GAME_WIDTH - 1;
                else
                    cursorX--;
                DrawCursor();
                break;
            case KEY_RIGHT_INKEY:
                ClearCursor();
                if (cursorX == GAME_WIDTH - 1)
                    cursorX = 0;
                else
                    cursorX++;
                DrawCursor();
                break;
            case KEY_ENTER:
            case ' ':
                MoveBall();
                break;
        }
    }
    return 0;
}
