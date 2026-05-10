/*
 * c8080 stdlib
 * Copyright (c) 2026 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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

#include <c8080/strtoi.h>
#include <c8080/strtou.h>
#include <stdbool.h>
#include <stddef.h>
#include <errno.h>
#include <limits.h>

#define ISSPACE(C) ((C) == ' ' || ((C) >= 0x09 && (C) <= 0x0A))

static unsigned strtoui(const char *str, char **endptr, int base, bool signed_result) {
    if (endptr != NULL)
        *endptr = (char *)str;

    while (ISSPACE(*str))
        str++;

    bool neg;
    if (*str == '-') {
        str++;
        neg = true;
    } else {
        if (*str == '+')
            str++;
        neg = false;
    }

    if ((base == 0 || base == 16) && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
        base = 16;
    } else if (base == 0) {
        base = str[0] == '0' ? 8 : 10;
    } else if (base < 0 || base == 1 || base > 36) {
        errno = EINVAL;
        return 0;
    }

    const unsigned limit = (unsigned)UINT_MAX / (unsigned)base;
    unsigned result = 0;
    bool overflow = false;
    for (;;) {
        uint8_t c = *str;
        if (c >= 'A') {
            if (c >= 'a')
                c -= 'a' - 10;
            else
                c -= 'A' - 10;
        } else {
            c -= '0';
            if (c > 9)
                break;
        }
        if (c >= (uint8_t)base)
            break;

        if (result > limit)
            overflow = true;
        result = result * base + c;
        if (result < c)
            overflow = true;
        str++;
        if (endptr != NULL)
            *endptr = (char *)str;
    }

    if (signed_result) {
        if (neg) {
            result = -result;
            if ((int)result > 0)
                overflow = true;
        } else {
            if ((int)result < 0)
                overflow = true;
        }

        if (overflow) {
            errno = ERANGE;
            return INT_MAX;
        }
    } else if (neg || overflow) {
        errno = ERANGE;
        return UINT_MAX;
    }

    return result;
}

int strtoi(const char *str, char **endptr, int base) {
    return (int)strtoui(str, endptr, base, true);
}

unsigned strtou(const char *str, char **endptr, int base) {
    return strtoui(str, endptr, base, false);
}


