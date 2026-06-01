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
#include <stddef.h>
#include <stdbool.h>
#include "arch.h"

static const uint8_t PANEL_OX = 2;
static const uint8_t PANEL_OY = 2;
static const uint8_t PANEL_COLUMN_WIDTH = 15;
static const uint8_t PANEL_COLUMNS_COUNT = 2;
static const uint8_t PANEL_ROWS_COUNT = TEXT_HEIGHT - 8;
static const uint8_t PANEL_WIDTH = TEXT_WIDTH / 2;
static const uint8_t PANEL_SHORT_PATH = PANEL_WIDTH - 4;

struct FileInfo {
    char name83[8 + 3];
    uint8_t attrib;
    uint16_t blocks_128;
};

static const uint8_t ATTRIB_SHIFT = 1;
static const uint8_t ATTRIB_DIR_UP = 1;
static const uint8_t ATTRIB_DIR_MAX = 0x0F;
static const uint8_t ATTRIB_DIR_SHIFT = 4;
static const uint8_t ATTRIB_DIR_MASK = ATTRIB_DIR_MAX << ATTRIB_DIR_SHIFT;
static const uint8_t ATTRIB_DIR_ALL = ATTRIB_DIR_MASK | ATTRIB_DIR_UP;
#define GET_DIR_FROM_ATTRIB(X) ((uint8_t)(X) >> ATTRIB_DIR_SHIFT)

struct Panel {
    uint8_t copy_start[0];
    uint8_t drive_user;
    char path[128];
    uint8_t short_path_skip;
    uint8_t short_path_size;
    uint16_t total_kb;
    uint16_t free_kb;
    uint16_t count;
    uint8_t copy_end[0];

    uint16_t offset_now;
    uint8_t cursor_x_now;
    uint8_t cursor_y_now;
    uint16_t offset;
    uint8_t cursor_x;
    uint8_t cursor_y;
    struct FileInfo *files;
    char selected_name[8 + 1 + 3 + 1];
};

extern struct Panel panel_a, panel_b;
extern size_t panel_files_max;
extern uint8_t panel_x;
extern uint8_t panel_reload_select_dir;  // Глобальная переменная для оптимизации размера

#ifdef FULL_COLOR_MODE
void PanelDrawBorder(uint8_t x);
#endif

void PanelReloadOrCopy(void);
void PanelReload(void);
uint8_t PanelGetDrive(void);
uint8_t PanelGetDirIndex(void);
void PanelDrawFreeSpace(void);
void PanelDrawFileInfo(void);
void PanelDrawTitle(uint8_t color);
void PanelRedrawCursor(bool show);
void PanelDrawFiles(bool active);
void PanelMoveCursorLeft(void);
void PanelMoveCursorRight(void);
void PanelMoveCursorUp(void);
void PanelMoveCursorDown(void);
uint16_t PanelGetCursorIndex(void);
struct FileInfo *PanelGetCursor(void);
void PanelSwap(void);
