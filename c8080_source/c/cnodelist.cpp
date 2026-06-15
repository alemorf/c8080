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

#include "cnodelist.h"
#include "cnode.h"

void CNodeList::PushBack(const CNodePtr &item) {
    if (first == nullptr) {
        first = item;
        last = item;
        if (last != nullptr)
            while (last->next_node != nullptr)
                last = last->next_node;
    } else {
        last->next_node = item;
        while (last->next_node != nullptr)
            last = last->next_node;
    }
}
