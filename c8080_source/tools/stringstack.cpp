/*
 * c8080 compiler
 * Copyright (c) 2026 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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

#include "stringstack.h"
#include <string.h>
#include <assert.h>

const char *StringStack::Save(const char *data, size_t size) {
    if (size == 0)
        return "";

    assert(data != nullptr);

    const size_t size_with_term = size + 1;

    if (size_with_term > sizeof(Item::data)) {
        huge_strings.emplace_back();
        std::vector<char> &new_string = huge_strings.back();
        new_string.resize(size_with_term);
        char *const r = new_string.data();
        memcpy(r, data, size);
        r[size] = 0;
        return r;
    }

    if (items.empty() || size_with_term > sizeof(Item::data) - items.back().used)
        items.emplace_back();

    Item &i = items.back();
    char *const r = i.data + i.used;
    assert(i.used <= sizeof(Item::data) && size_with_term <= sizeof(Item::data) - i.used);
    memcpy(r, data, size);
    r[size] = 0;
    i.used += size_with_term;

    return r;
}
