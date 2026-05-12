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

void __global CpmConvertFromName83(char *out_name, const char *name83) {
    asm {
__a_2_cpmconvertfromname83 = 0
        ex   hl, de
__a_1_cpmconvertfromname83 = $ + 1
        ld   bc, 0

        ld   hl, bc
        ld   a, 8
        call CpmConvertFromName83_1

        ld   hl, bc
        ld   (hl), '.'
        inc  hl
        ld   a, 3
        jp   CpmConvertFromName83_1

        ; Copy chars
CpmConvertFromName83_1:
        push af
        ld   a, (de)
        inc  de
        and  7Fh
        sub  1  ; Replace 0 (teminator) by 1
        adc  1
        ld   (hl), a
        inc  hl
        cp   ' '
        jp   z, CpmConvertFromName83_2
        ld   bc, hl
CpmConvertFromName83_2:
        pop  af
        dec  a
        jp   nz, CpmConvertFromName83_1

        ld   (bc), a
    }
}
