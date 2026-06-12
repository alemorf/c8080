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

#pragma once

#include <list>
#include <vector>
#include <stddef.h>

class StringStack {
private:
    struct Item {
        size_t used{};
        char data[0x10000];
    };

    std::list<std::vector<char>> huge_strings;
    std::list<Item> items;

    StringStack(const StringStack &) = delete;
    StringStack(StringStack &&) = delete;
    StringStack &operator=(const StringStack &) = delete;
    StringStack &operator=(StringStack &&) = delete;

public:
    StringStack() = default;
    const char *Save(const char *data, size_t size);
};
