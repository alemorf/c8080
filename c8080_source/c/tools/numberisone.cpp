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

#include "numberiszero.h"
#include "cthrow.h"

bool NumberIsOne(CConstNodePtr node) {
    assert(node != nullptr);

    if (node->type == CNT_NUMBER) {
        if (node->ctype.IsPointer())
            return node->number.u == 0;
        switch (node->ctype.base_type) {
            case CBT_CHAR:
            case CBT_SIGNED_CHAR:
            case CBT_SHORT:
            case CBT_INT:
            case CBT_LONG:
            case CBT_LONG_LONG:
                return node->number.i == 1;
            case CBT_UNSIGNED_CHAR:
            case CBT_UNSIGNED_SHORT:
            case CBT_UNSIGNED_INT:
            case CBT_UNSIGNED_LONG:
            case CBT_UNSIGNED_LONG_LONG:
                return node->number.u == 1;
            case CBT_FLOAT:
                return node->number.f == 1;
            case CBT_DOUBLE:
                return node->number.d == 1;
            case CBT_LONG_DOUBLE:
                return node->number.ld == 1;
        }
    }

    return false;
}
