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

#include "loguint64.h"
#include <limits.h>

uint8_t LogUint64(uint64_t number) {
    switch (number) {
        case 0x0001ULL:
            return 0;
        case 0x0002ULL:
            return 1;
        case 0x0004ULL:
            return 2;
        case 0x0008ULL:
            return 3;
        case 0x0010ULL:
            return 4;
        case 0x0020ULL:
            return 5;
        case 0x0040ULL:
            return 6;
        case 0x0080ULL:
            return 7;
        case 0x0100ULL:
            return 8;
        case 0x0200ULL:
            return 9;
        case 0x0400ULL:
            return 10;
        case 0x0800ULL:
            return 11;
        case 0x1000ULL:
            return 12;
        case 0x2000ULL:
            return 13;
        case 0x4000ULL:
            return 14;
        case 0x8000ULL:
            return 15;
        case 0x10000ULL:
            return 16;
        case 0x20000ULL:
            return 17;
        case 0x40000ULL:
            return 18;
        case 0x80000ULL:
            return 19;
        case 0x100000ULL:
            return 20;
        case 0x200000ULL:
            return 21;
        case 0x400000ULL:
            return 22;
        case 0x800000ULL:
            return 23;
        case 0x1000000ULL:
            return 24;
        case 0x2000000ULL:
            return 25;
        case 0x4000000ULL:
            return 26;
        case 0x8000000ULL:
            return 27;
        case 0x10000000ULL:
            return 28;
        case 0x20000000ULL:
            return 29;
        case 0x40000000ULL:
            return 30;
        case 0x80000000ULL:
            return 31;
        case 0x100000000ULL:
            return 32;
        case 0x200000000ULL:
            return 33;
        case 0x400000000ULL:
            return 34;
        case 0x800000000ULL:
            return 35;
        case 0x1000000000ULL:
            return 36;
        case 0x2000000000ULL:
            return 37;
        case 0x4000000000ULL:
            return 38;
        case 0x8000000000ULL:
            return 39;
        case 0x10000000000ULL:
            return 40;
        case 0x20000000000ULL:
            return 41;
        case 0x40000000000ULL:
            return 42;
        case 0x80000000000ULL:
            return 43;
        case 0x100000000000ULL:
            return 44;
        case 0x200000000000ULL:
            return 45;
        case 0x400000000000ULL:
            return 46;
        case 0x800000000000ULL:
            return 47;
        case 0x1000000000000ULL:
            return 48;
        case 0x2000000000000ULL:
            return 49;
        case 0x4000000000000ULL:
            return 50;
        case 0x8000000000000ULL:
            return 51;
        case 0x10000000000000ULL:
            return 52;
        case 0x20000000000000ULL:
            return 53;
        case 0x40000000000000ULL:
            return 54;
        case 0x80000000000000ULL:
            return 55;
        case 0x100000000000000ULL:
            return 56;
        case 0x200000000000000ULL:
            return 57;
        case 0x400000000000000ULL:
            return 58;
        case 0x800000000000000ULL:
            return 59;
        case 0x1000000000000000ULL:
            return 60;
        case 0x2000000000000000ULL:
            return 61;
        case 0x4000000000000000ULL:
            return 62;
        case 0x8000000000000000ULL:
            return 63;
        // TODOULL: __builtin_clzll
        default:
            return UINT8_MAX;
    }
}
