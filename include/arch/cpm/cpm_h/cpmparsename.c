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

uint8_t __global CpmParseName(struct FCB *fcb, const char *name) {
    asm {
__a_2_cpmparsename = 0
__a_1_cpmparsename = $ + 1
        ld   de, 0

        ; Parse X:
        inc  hl
        ld   a, (hl)
        dec  hl
        cp   ':'
        jp   nz, cpmparsename_0
        ld   a, (hl)
        sub  'A' - 1 ; Drive @:
        cp   16
        jp   nc, cpmparsename_0
        ld   (de), a
        inc  hl
        inc  hl
cpmparsename_0:
        inc  de

        ; Name
        ex   hl, de
        ld   c, 8
        call cpmparsename_1

        ; Ext
        ld   c, 3
cpmparsename_1:
        ld   (hl), ' '
        ld   a, (de)
        or   a
        ret  m
        jp   z, cpmparsename_2
        cp   '.'
        jp   z, cpmparsename_2
        inc  de
        ld   (hl), a
cpmparsename_2:
        inc  hl
        dec  c
        jp   nz, cpmparsename_1

cpmparsename_3:
        ld   a, (de)
        or   a
        ret  z                   ; exit with A = 0
        sub  '.'
        inc  de
        jp   nz, cpmparsename_3  ; exit with A = 0
    }
}
