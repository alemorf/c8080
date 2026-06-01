/*
 * NC shell for CP/M
 * Copyright (c) 2026 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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

#include <c8080/colors.h>

extern uint8_t common_folder __address(0xFF77);        // TODO: Replace by ESC
extern uint8_t common_dont_exec_nc __address(0xFF78);  // TODO: Replace by ESC

static const uint8_t COLOR_COMMAND_LINE = COLOR_PAPER_BLACK | COLOR_INK_WHITE;
static const uint8_t COLOR_HELP_LINE = COLOR_PAPER_CYAN | COLOR_INK_BLACK;
static const uint8_t COLOR_PANEL_BORDER = COLOR_PAPER_BLUE | COLOR_INK_WHITE;
static const uint8_t COLOR_PANEL_TITLE = COLOR_PAPER_BLUE | COLOR_INK_WHITE;
static const uint8_t COLOR_PANEL_TITLE_ACTIVE = COLOR_PAPER_CYAN | COLOR_INK_BLACK;
static const uint8_t COLOR_PANEL_CURSOR = COLOR_PAPER_CYAN | COLOR_INK_BLACK;
static const uint8_t COLOR_WINDOW = COLOR_PAPER_WHITE | COLOR_INK_BLACK;
static const uint8_t COLOR_ERROR_WINDOW = COLOR_PAPER_WHITE | COLOR_INK_BLACK;
static const uint8_t COLOR_INPUT = COLOR_PAPER_BLACK | COLOR_INK_WHITE;
static const uint8_t COLOR_BUTTON = COLOR_PAPER_BLACK | COLOR_INK_WHITE;
static const uint8_t COLOR_BUTTON_ACTIVE = COLOR_PAPER_BLACK | COLOR_INK_WHITE;
