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

static const uint8_t KEY_F1 = 0x00;
static const uint8_t KEY_F2 = 0x01;
static const uint8_t KEY_F3 = 0x02;
static const uint8_t KEY_F4 = 0x03;
static const uint8_t KEY_F5 = 0x04;
static const uint8_t KEY_F6 = 0x05;
static const uint8_t KEY_F7 = 0x06;
static const uint8_t KEY_F8 = 0x07;
static const uint8_t KEY_LEFT = 0x08;
static const uint8_t KEY_LEFT_INKEY = 0x08;
static const uint8_t KEY_TAB = 0x09;
static const uint8_t KEY_PS = 0x0A;
static const uint8_t KEY_F9 = 0x0B; /* Internal driver */
static const uint8_t KEY_HOME = 0x0C;
static const uint8_t KEY_ENTER = 0x0D;
static const uint8_t KEY_F10 = 0x0E; /* Internal driver */
static const uint8_t KEY_F11 = 0x0F; /* Internal driver */
static const uint8_t KEY_PV = 0x10;  /* Internal driver */
static const uint8_t KEY_RUS = 0x11; /* Internal driver */
static const uint8_t KEY_UP = 0x19;
static const uint8_t KEY_UP_INKEY = 0x19;
static const uint8_t KEY_RIGHT = 0x18;
static const uint8_t KEY_RIGHT_INKEY = 0x18;
static const uint8_t KEY_DOWN = 0x1A;
static const uint8_t KEY_DOWN_INKEY = 0x1A;
static const uint8_t KEY_ESC = 0x1B;
static const uint8_t KEY_F12 = 0x1F;
static const uint8_t KEY_SPACE = 0x20;
static const uint8_t KEY_BACKSPACE = 0x7F; /* Internal driver */
static const uint8_t KEY_BACKSPACE_INKEY = 0x7F; /* Internal driver */
static const uint8_t KEY_F9_BIOS = 0x8A;   /* BIOS */
