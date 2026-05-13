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

#include <stdio.h>
#include <stdint.h>
#include <cpm.h>

void WriteConsole(const char *text) {
    while (*text) {
#ifdef __C8080_USE_CPM_CONSOLE_IO /* Cannot use CpmBiosConSt() because symbols get stuck in CP/M */
        if (*text == 0x0A)
            CpmConsoleWrite(0x0D);
        CpmConsoleWrite(*text);
#else
        if (*text == 0x0A)
            CpmBiosConOut(0x0D);
        CpmBiosConOut(*text);
#endif
        text++;
    }
}
