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

void __global MoveCursor(uint8_t x, uint8_t y) {
    asm {
__a_2_movecursor = 0
	add  ' '
	ld   (movecursor_data + 3), a
	
__a_1_movecursor = $+1
        ld   a, 0
	add  ' '
	ld   (movecursor_data + 2), a
	
	ld   hl, movecursor_data
        jp   0F818h
movecursor_data:
	.db 1Bh, 'Y', 20h, 20h, 0
    }
}
