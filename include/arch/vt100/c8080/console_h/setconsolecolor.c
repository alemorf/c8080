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

#include <c8080/console.h>

void SetConsoleColor(uint8_t color) {
    static char str[] = "\x1B[0m\x1B[30m\x1B[040m";
    str[6] = (color & 0x08) ? '9' : '3';
    str[7] = (color & 0x07) + '0';
    if (color & 0x80) {
        str[11] = '1';
        str[12] = '0';
    } else {
        str[11] = '0';
        str[12] = '4';
    }
    str[13] = ((color >> 4) & 0x07) + '0';
    WriteConsole(str);
}
