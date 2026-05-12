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

#include <cpm.h>

#if !defined(ARCH_CPM_BIOS) && !defined(ARCH_CPM_BDOS)

void __global CpmCommand(uint8_t drive_user, const char *text) {
    asm {
        ; Адрес строки с командой в DE
__a_2_cpmcommand = 0
        ex   hl, de

        ; Вычисление адреса CCP
        ld   a, (7)
        sub  8
        ld   c, a

        ; Сброс указателя командной строки в CCP
        ld   h, c
        ld   l, 88h
        ld   (hl), 8
        inc  l
        ld   (hl), h

        ; Передача командной строки
        ld   l, 8
CpmCommand_1:
        ld   a, l
        cp   88h
        jp   z, 0  ; Перезагрузка, если слишком длинная ком строка
        ld   a, (de)
        inc  de
        ld   (hl), a
        inc  l
        or   a
        jp   nz, CpmCommand_1

        ; Передача длины команды
        ld   a, l
        sub  9
        ld   l, 7
        ld   (hl), a

        ; Передача активного накопителя и пользователя в C
__a_1_cpmcommand = $ + 1
        ld   c, 0

        ; Запуск CCP
        ld   l, 0
        jp   (hl)
    }
}

#endif
