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

uint16_t __global CpmGetNameAndAttrib(void *, const void *) {
    asm {
__a_2_cpmgetnameandattrib = 0
        ex   hl, de
__a_1_cpmgetnameandattrib = $ + 1
        ld   bc, 0
        ld   hl, 0
        ld   a, 8 + 3
CpmGetNameAndAttrib_1:
        push af
        add  hl, hl

        ld   a, (de)
        add  a
        ld   a, l
        adc  0
        ld   l, a

        ld   a, (de)
        inc  de
        and  7Fh
        ld   (bc), a
        inc  bc

        pop  af
        dec  a
        jp   nz, CpmGetNameAndAttrib_1
    }
}
