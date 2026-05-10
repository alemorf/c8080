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

#pragma once

#include <stdint.h>
#include <c8080/colors.h>

static const uint8_t COLOR_STEP = 13;

static const uint8_t PLAYFIELD_Y = 28;
static const uint8_t PLAYFIELD_X = 11;
static const uint8_t CELL_TILE_WIDTH = 3;
static const uint8_t CELL_TILE_HEIGHT = 20;

static const uint8_t SCORE_COLOR = COLOR_INK_GREEN;
static const uint8_t SCORE_X = 162;
static const uint8_t SCORE_Y = 8;

static const uint8_t PLAYAR_MAX_HEIGHT = 14;
static const uint8_t PLAYER_Y = 172;
static const uint8_t PLAYER_X = 40;
static const uint8_t PLAYER_CROWN_DX = 2;
static const uint8_t PLAYER_CROWN_DY = 4;

static const uint8_t KING_X = 3;
static const uint8_t KING_Y = 53;

static const uint8_t HIRECT_X = 13;
static const uint8_t HIRECT_Y = 46;
static const uint8_t HIRECT_WIDTH = 6;
static const uint8_t HIRECT_HEIGHT = 7;

static const uint8_t HI_X = 72;
static const uint8_t HI_Y = 57;
static const uint8_t HI_LINE_HEIGHT = 10;
static const uint8_t HI_COLOR_0 = COLOR_INK_CYAN;
static const uint8_t HI_COLOR_1 = COLOR_INK_RED;
static const uint8_t HI_COLOR_2 = COLOR_INK_YELLOW;
static const uint8_t HI_COLOR_3 = COLOR_INK_WHITE;

static const uint8_t BUTTON_PATH_X = 4;
static const uint8_t BUTTON_PATH_Y = 240;
static const uint8_t BUTTON_PATH_W = 4;
static const uint8_t BUTTON_PATH_H = 8;
static const uint8_t BUTTON_SOUND_X = 13;
static const uint8_t BUTTON_SOUND_Y = 240;
static const uint8_t BUTTON_SOUND_W = 4;
static const uint8_t BUTTON_SOUND_H = 8;
static const uint8_t BUTTON_HELP_X = 22;
static const uint8_t BUTTON_HELP_Y = 240;
static const uint8_t BUTTON_HELP_W = 4;
static const uint8_t BUTTON_HELP_H = 8;
static const uint8_t BUTTON_COLOR_ENABLED = COLOR_INK_GREEN;
static const uint8_t BUTTON_COLOR_DISABLED = COLOR_INK_BLUE;

static const uint8_t HELP_1_X = 20;
static const uint8_t HELP_1_Y = 3;
static const uint8_t HELP_2_X = 23;
static const uint8_t HELP_2_Y = 3;
static const uint8_t HELP_3_X = 26;
static const uint8_t HELP_3_Y = 3;

static const uint8_t TOPSCORE_X = 16;
static const uint8_t TOPSCORE_Y = 8;
static const uint8_t TOPSCORE_COLOR = COLOR_INK_GREEN;

static const uint8_t TOPNAME_X = 9;
static const uint8_t TOPNAME_Y = 190;
static const uint8_t TOPNAME_COLOR = COLOR_INK_GREEN;

static const uint8_t all_colors[] = {
    COLOR_INK_BLACK,  COLOR_INK_RED,  COLOR_INK_GREEN,  COLOR_INK_BLUE,
    COLOR_INK_YELLOW, COLOR_INK_CYAN, COLOR_INK_VIOLET, COLOR_INK_WHITE,
};

#define BALL_COLOR(X) (all_colors[(X)])
