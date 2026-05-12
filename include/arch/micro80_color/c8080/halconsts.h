// c8080 stdlib
// Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include <c8080/codepage/micro80.h>

#define FEATURE_HAL_SAVE_SCREEN

static uint8_t *const SCREEN = (uint8_t *)0xE800;
static uint8_t *const SCREEN_ATTRIB = (uint8_t *)0xE000;
static const uint16_t SCREEN_SIZE = 0x800;
static const uint8_t SCREEN_BPL = 64;
static const uint8_t TEXT_WIDTH = 64;
static const uint8_t TEXT_HEIGHT = 25;
static const uint8_t TILE_WIDTH = 64;
static const uint8_t TILE_HEIGHT = 25;
static const uint8_t SCREEN_WIDTH = TILE_WIDTH * 2;
static const uint8_t SCREEN_HEIGHT = TILE_HEIGHT * 2;

struct SavedScreen {
    char screen[TEXT_WIDTH * TEXT_HEIGHT * 2];
    uint16_t cursor;
    uint8_t cursor_visible;
    uint8_t color;
};

#define TILE(X, Y) (SCREEN + (X) + (Y)*SCREEN_BPL)
#define DRAWTEXTARGS(X, Y) (SCREEN + (X) + (Y)*SCREEN_BPL), 0

uint8_t ReadKey(void) __address(0xF803);
uint8_t InKey(void) __address(0xF81B);