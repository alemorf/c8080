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

#include "uint16tostring.h"
#include <c8080/remainder.h>

char *Uint16ToString(char *outputBuffer, uint16_t value, uint8_t radix) {
    uint8_t i = UINT16_TO_STRING_SIZE - 1;
    do {
        *outputBuffer = ' ';
        ++outputBuffer;
        i--;
    } while (i != 0);
    *outputBuffer = '\0';
    do {
        value /= radix;
        --outputBuffer;
        char c = __remainder + '0';
        if (c > '9')
            c += 'A' - '0' - 10;
        *outputBuffer = c;
    } while (value != 0);
    return outputBuffer;
}
