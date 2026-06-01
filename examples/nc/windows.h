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

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "arch.h"

static const uint8_t WINDOW_WIDTH = 24;
static const uint8_t WINDOW_X_LEFT = 2;
static const uint8_t WINDOW_X_CENTER = (TEXT_WIDTH - WINDOW_WIDTH - 4) / 2;
static const uint8_t WINDOW_X_RIGHT = TEXT_WIDTH / 2 + 2;
static const uint8_t PROGRESS_WIDTH = WINDOW_WIDTH;
static const uint8_t PROGRESS_X = (TEXT_WIDTH - PROGRESS_WIDTH) / 2;

extern char input[128];
extern uint8_t input_size;
extern uint8_t window_color;

uint8_t DrawWindow(uint8_t x, uint8_t height, const char *title);
void DrawWindowText(uint8_t y, const char *text);
void DrawWindowTextCenter(uint8_t y, const char *text);
uint8_t DrawButtons(uint8_t y, uint8_t cursor, const char *items);
uint8_t RunButtons(uint8_t y, uint8_t cursor, const char *items);
void DrawInput(uint8_t x, uint8_t y, uint8_t width, uint8_t color);
void DrawInputCursor(void);
void ProcessInput(int c);
bool RunInput(uint8_t y);
void DrawProgress(uint8_t y);
void DrawProgressNext(uint8_t y, uint16_t current, uint16_t maximal);
char *MakeString(char *str, char c, uint8_t l);

bool DeleteWindow(const char *file_name);
void ErrorWindow(const char *text);
uint8_t SelectDriveWindow(uint8_t cursor, bool is_right);
bool MakeDirWindow(void);
