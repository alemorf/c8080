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
#include "cerrorposition.h"

enum CBaseType {
    CBT_STRUCT = 0,
    CBT_FUNCTION = 1,
    CBT_VOID = 2,
    CBT_CHAR = 3,
    CBT_UNSIGNED_CHAR = 4,
    CBT_SIGNED_CHAR = 5,
    CBT_SHORT = 6,
    CBT_UNSIGNED_SHORT = 7,
    CBT_INT = 8,
    CBT_UNSIGNED_INT = 9,
    CBT_LONG = 10,
    CBT_UNSIGNED_LONG = 11,
    CBT_LONG_LONG = 12,
    CBT_UNSIGNED_LONG_LONG = 13,
    CBT_FLOAT = 14,
    CBT_DOUBLE = 15,
    CBT_LONG_DOUBLE = 16,
    CBT_VA_LIST = 17,

    CBT_BOOL = CBT_UNSIGNED_CHAR,
    CBT_SIZE = CBT_UNSIGNED_INT,
    CBT_INT8 = CBT_CHAR,
    CBT_UINT8 = CBT_UNSIGNED_CHAR,
    CBT_INT16 = CBT_SHORT,
    CBT_UINT16 = CBT_UNSIGNED_SHORT,
    CBT_INT32 = CBT_LONG,
    CBT_UINT32 = CBT_UNSIGNED_LONG,
    CBT_INT64 = CBT_LONG_LONG,
    CBT_UINT64 = CBT_UNSIGNED_LONG_LONG,
};

uint8_t SizeOf(CBaseType type, CConstErrorPosition e);
bool IsInteger(CBaseType type);
bool IsUnsigned(CBaseType type);
CBaseType CalcResultCBaseType(CBaseType a, CBaseType b);
