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

static const uint16_t KEY_UP = 0x200 | 'A';     /* ESC [ A */
static const uint16_t KEY_DOWN = 0x200 | 'B';   /* ESC [ B */
static const uint16_t KEY_RIGHT = 0x200 | 'C';  /* ESC [ C */
static const uint16_t KEY_LEFT = 0x200 | 'D';   /* ESC [ D */
static const uint16_t KEY_END = 0x200 | 'F';    /* ESC [ F */
static const uint16_t KEY_HOME = 0x200 | 'H';   /* ESC [ H */

static const uint8_t KEY_ENTER = 0x0D;
static const uint8_t KEY_SPACE = 0x20;
static const uint8_t KEY_BACKSPACE = 0x08;
static const uint8_t KEY_TAB = 0x09;
static const uint8_t KEY_ESC = 0x0C;
