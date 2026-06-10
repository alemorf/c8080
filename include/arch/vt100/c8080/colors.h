/*
 * c8080 stdlib
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>

static const uint8_t COLOR_PAPER_LIGHT = 1 << 7;
static const uint8_t COLOR_INK_LIGHT = 1 << 3;

static const uint8_t COLOR_INK_BLACK = 0;
static const uint8_t COLOR_INK_RED = 1;
static const uint8_t COLOR_INK_GREEN = 2;
static const uint8_t COLOR_INK_YELLOW = 3;
static const uint8_t COLOR_INK_BLUE = 4;
static const uint8_t COLOR_INK_CYAN = 6;
static const uint8_t COLOR_INK_MAGENTA = 5;
static const uint8_t COLOR_INK_WHITE = 7;

static const uint8_t COLOR_INK_GRAY = COLOR_INK_LIGHT | COLOR_INK_BLACK;
static const uint8_t COLOR_INK_LIGHT_BLUE = COLOR_INK_LIGHT | COLOR_INK_BLUE;
static const uint8_t COLOR_INK_LIGHT_RED = COLOR_INK_LIGHT | COLOR_INK_RED;
static const uint8_t COLOR_INK_LIGHT_CYAN = COLOR_INK_LIGHT | COLOR_INK_CYAN;
static const uint8_t COLOR_INK_LIGHT_GREEN = COLOR_INK_LIGHT | COLOR_INK_GREEN;
static const uint8_t COLOR_INK_LIGHT_MAGENTA = COLOR_INK_LIGHT | COLOR_INK_MAGENTA;
static const uint8_t COLOR_INK_LIGHT_YELLOW = COLOR_INK_LIGHT | COLOR_INK_YELLOW;
static const uint8_t COLOR_INK_LIGHT_WHITE = COLOR_INK_LIGHT | COLOR_INK_WHITE;

static const uint8_t COLOR_PAPER_BLACK = COLOR_INK_BLACK << 4;
static const uint8_t COLOR_PAPER_RED = COLOR_INK_RED << 4;
static const uint8_t COLOR_PAPER_BLUE = COLOR_INK_BLUE << 4;
static const uint8_t COLOR_PAPER_CYAN = COLOR_INK_CYAN << 4;
static const uint8_t COLOR_PAPER_GREEN = COLOR_INK_GREEN << 4;
static const uint8_t COLOR_PAPER_MAGENTA = COLOR_INK_MAGENTA << 4;
static const uint8_t COLOR_PAPER_YELLOW = COLOR_INK_YELLOW << 4;
static const uint8_t COLOR_PAPER_WHITE = COLOR_INK_WHITE << 4;

static const uint8_t COLOR_PAPER_GRAY = COLOR_INK_GRAY << 4;
static const uint8_t COLOR_PAPER_LIGHT_RED = COLOR_INK_LIGHT_RED << 4;
static const uint8_t COLOR_PAPER_LIGHT_BLUE = COLOR_INK_LIGHT_BLUE << 4;
static const uint8_t COLOR_PAPER_LIGHT_CYAN = COLOR_INK_LIGHT_CYAN << 4;
static const uint8_t COLOR_PAPER_LIGHT_GREEN = COLOR_INK_LIGHT_GREEN << 4;
static const uint8_t COLOR_PAPER_LIGHT_MAGENTA = COLOR_INK_LIGHT_MAGENTA << 4;
static const uint8_t COLOR_PAPER_LIGHT_YELLOW = COLOR_INK_LIGHT_YELLOW << 4;
static const uint8_t COLOR_PAPER_LIGHT_WHITE = COLOR_INK_LIGHT_WHITE << 4;
