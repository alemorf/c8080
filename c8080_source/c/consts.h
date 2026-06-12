/*
 * c8080 compiler
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

static constexpr uint8_t C_SIZEOF_POINTER = 2;
static constexpr uint8_t C_SIZEOF_CHAR = 1;
static constexpr uint8_t C_SIZEOF_SHORT = 2;
static constexpr uint8_t C_SIZEOF_INT = 2;
static constexpr int16_t C_INT_MIN = (int16_t)-0x8000;
static constexpr int16_t C_INT_MAX = 0x7FFF;
static constexpr uint16_t C_UINT_MAX = 0xFFFFu;
static constexpr int32_t C_LONG_MAX = 0x7FFFFFFF;
static constexpr uint32_t C_ULONG_MAX = 0xFFFFFFFFu;
static constexpr uint8_t C_SIZEOF_LONG = 4;
static constexpr uint8_t C_SIZEOF_LONG_LONG = 8;
static constexpr uint8_t C_SIZEOF_FLOAT = 4;
static constexpr uint8_t C_SIZEOF_DOUBLE = 8;
static constexpr uint8_t C_SIZEOF_LONG_DOUBLE = 16;
static constexpr uint16_t C_SIZE_MAX = 0xFFFFu;
