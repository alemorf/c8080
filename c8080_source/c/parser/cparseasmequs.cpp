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

#include "cparseasmequs.h"

void CParseAsmEqus(const char *str, std::set<CString> &out_values) {
    const char *p = str;
    for (;;) {
        if (isalnum(static_cast<unsigned char>(*p)) || *p == '_') {
            const char *line_start = p;
            do {
                p++;
            } while (isalnum(static_cast<unsigned char>(*p)) || *p == '_');
            const char *is_end = p;
            while (*p == ' ')
                p++;
            if (*p == '=' || *p == ':')
                out_values.insert(CString(line_start, is_end - line_start));
        }
        p = strchr(p, '\n');
        if (p == nullptr)
            break;
        p++;
    }
}
