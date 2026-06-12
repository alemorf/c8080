/*
 * NC shell for CP/M
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

#include "windows.h"
#include <c8080/console.h>
#include <c8080/hal.h>
#include <c8080/keys.h>
#include <string.h>
#include "stdio.h"
#include "config.h"

uint8_t window_color = COLOR_WINDOW;
uint8_t window_x = 0;
char input[128];
uint8_t input_size;
static uint8_t input_cursor_x, input_cursor_y;

char *MakeString(char *str, char c, uint8_t l) {
    memset(str, c, l);
    str[l] = 0;
    return str;
}

void DrawWindowTextCenter(uint8_t y, const char *text) {
    DrawTextXY((WINDOW_WIDTH - strlen(text)) / 2 + window_x, y, window_color, text);
}

void DrawWindowText(uint8_t y, const char *text) {
    DrawTextXY(window_x, y, window_color, text);
}

uint8_t DrawWindow(uint8_t x, uint8_t height, const char *title) {
    HideCursor();
    window_x = x;
    uint8_t y = ((TEXT_HEIGHT - 6) - height) / 2U;
    const uint8_t result = y;
    DrawWindowText(y, "╔══════════════════════════╗");
    y++;
#ifdef FEATURE_HAL_CHANGE_TILE_COLOR
    ChangeTileColor(TILE(window_x + (WINDOW_WIDTH + 4), y), COLOR_WINDOW_SHADOW, 2, height + 2);
#endif
    do {
        DrawWindowText(y, "║                          ║");
        y++;
        height--;
    } while (height != 0);
    DrawWindowText(y, "╚══════════════════════════╝");
#ifdef FEATURE_HAL_CHANGE_TILE_COLOR
    ChangeTileColor(TILE(window_x + 2, y + 1), COLOR_WINDOW_SHADOW, WINDOW_WIDTH + 4, 1);
#endif
    window_x += 2;
    DrawWindowTextCenter(result, title);
    return result + 2;
}

void DrawInputCursor(void) {
    MoveCursor(input_cursor_y, input_cursor_x);
    ShowCursor();
}

void DrawInput(uint8_t x, uint8_t y, uint8_t width, uint8_t color) {
    char spaces[TEXT_WIDTH + 1];
    MakeString(spaces, ' ', width);

    uint8_t offset = 0;
    if (input_size >= width)
        offset = input_size - width + 1;
    memcpy(spaces, input + offset, input_size - offset);

    DrawTextXY(x, y, color, spaces);
    input_cursor_x = x + input_size - offset;
    input_cursor_y = y;
}

void ProcessInput(int c) {
    if (c == KEY_BACKSPACE) {
        if (input_size > 0)
            input_size--;
    } else if (c >= ' ' && c <= 0xFF) {
        if (input_size < sizeof(input) - 2) {
            input[input_size] = c;
            input_size++;
        }
    }
    input[input_size] = 0;
}

bool RunInput(uint8_t y) {
    input_size = 0;  // TODO: input_pos = strlen(input);
    int c;
    for (;;) {
        DrawInput(window_x, y, WINDOW_WIDTH, COLOR_INPUT);
        DrawInputCursor();
        c = ReadAndDecodeConsoleKeys();
        if (c == KEY_ENTER)
            break;
        if (c == KEY_ESC)
            break;
        ProcessInput(c);
    }
    input_size = 0;  // Что бы введенный текст не появился в ком. строке
    return c == KEY_ENTER;
}

void DrawProgress(uint8_t y) {
    char buf[PROGRESS_WIDTH + 1];
    DrawWindowText(y, MakeString(buf, '░', PROGRESS_WIDTH));
}

void DrawProgressNext(uint8_t y, uint16_t current, uint16_t maximal) {
    while (current > (uint16_t)(0xFFFF / PROGRESS_WIDTH)) {
        current /= 2;
        maximal /= 2;
    }
    const uint8_t width = current * PROGRESS_WIDTH / maximal;
    char buf[PROGRESS_WIDTH + 1];
    DrawWindowText(y, MakeString(buf, '█', width <= PROGRESS_WIDTH ? width : PROGRESS_WIDTH));
}

static uint8_t DrawButton(uint8_t x, uint8_t y, uint8_t active, const char *text, uint8_t text_size) {
    char buf[16];
    MakeString(buf, ' ', text_size + 4);

    if (active) {
        buf[0] = '[';
        buf[text_size + 3] = ']';
    }

    memcpy(buf + 2, text, text_size);
    DrawTextXY(x, y, active ? COLOR_BUTTON_ACTIVE : COLOR_BUTTON, buf);
#ifdef DRAW_BUTTON_SHADOW
    text_size += 4;
    DrawTextXY(x + text_size, y, CHANGE_INK_TO_BLACK(window_color), "▄");
    memset(buf, '▀', text_size);
    DrawTextXY(x + 1, y + 1, CHANGE_INK_TO_BLACK(window_color), buf);
#endif
}

uint8_t DrawButtons(uint8_t y, uint8_t cursor, const char *items) {
    uint8_t i;
    uint8_t x = -3;
    uint8_t step = 0;
    do {
        i = 0;
        const char *p = items;
        do {
            uint8_t l = strlen(p);
            if (step == 1)
                DrawButton(x, y, cursor == i, p, l);
            // В оригинале одна буква подсвечена
            i++;
            x += l + (4 + 3);
            p += l + 1;
        } while (*p);
        x = (uint8_t)(WINDOW_WIDTH - x) / 2 + window_x;
        step++;
    } while (step < 2);
    return i;
}

uint8_t RunButtons(uint8_t y, uint8_t cursor, const char *items) {
    for (;;) {
        const uint8_t count = DrawButtons(y, cursor, items);
        switch (ReadAndDecodeConsoleKeys()) {
            case KEY_LEFT:
                cursor--;
                if (cursor > count)
                    cursor = count - 1;
                break;
            case KEY_RIGHT:
                cursor++;
                if (cursor >= count)
                    cursor = 0;
                break;
            case KEY_ENTER:
                return cursor;
            case KEY_ESC:
                return -1;
        }
    }
}

bool DeleteWindow(const char *file_name) {
    const uint8_t y = DrawWindow(WINDOW_X_CENTER, 6, " Delete ");  // Original
    DrawWindowTextCenter(y, "Do you wish to delete");              // Original
    DrawWindowTextCenter(y + 1, file_name);
    return RunButtons(y + 3, 0, "Delete\0Cancel\0") == 0;  // Original: Delte Filters Cancel
}

void ErrorWindow(const char *text) {
    window_color = COLOR_ERROR_WINDOW;
    const uint8_t y = DrawWindow(WINDOW_X_CENTER, 5, " Error ");  // В оригинале тут заголовок прошлого окна
    DrawWindowTextCenter(y, text);                                // Original
    RunButtons(y + 2, 0, "Ok\0");                                 // Original
    window_color = COLOR_WINDOW;
}

bool MakeDirWindow(void) {
    const uint8_t y = DrawWindow(WINDOW_X_CENTER, 7, " Make directory ");  // Original
    DrawWindowText(y, "Create the directory");                             // Original
    DrawButtons(y + 4, 0, "Create\0");
    input[0] = 0;
    return RunInput(y + 2);
}

uint8_t SelectDriveWindow(uint8_t cursor, bool is_right) {
    const uint8_t y = DrawWindow(is_right ? WINDOW_X_RIGHT : WINDOW_X_LEFT, 5, " Drives ");  // Original: Drive letter
    DrawWindowTextCenter(y, is_right ? "Choose right drive:" : "Choose left drive:");        // Original
    return RunButtons(y + 2, cursor, DRIVES);
}
