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

#include "nodeisnumber.h"

bool NodeIsNumber1(CConstNodePtr node) {
    assert(node != nullptr);

    if (node->type == CNT_NUMBER) {
        switch (node->ctype.GetAsmType()) {
            case CBT_CHAR:
            case CBT_SHORT:
            case CBT_LONG:
            case CBT_LONG_LONG:
                return node->number.i == 1;
            case CBT_UNSIGNED_CHAR:
            case CBT_UNSIGNED_SHORT:
            case CBT_UNSIGNED_LONG:
            case CBT_UNSIGNED_LONG_LONG:
                return node->number.u == 1u;
            case CBT_FLOAT:
                return node->number.f == 1.0f;
            case CBT_DOUBLE:
                return node->number.d == 1.0;
            case CBT_LONG_DOUBLE:
                return node->number.ld == 1.0L;
        }
    }

    return false;
}

bool NodeIsNumber0(CConstNodePtr node) {
    assert(node != nullptr);

    if (node->type == CNT_NUMBER) {
        switch (node->ctype.GetAsmType()) {
            case CBT_CHAR:
            case CBT_SHORT:
            case CBT_LONG:
            case CBT_LONG_LONG:
                return node->number.i == 0;
            case CBT_UNSIGNED_CHAR:
            case CBT_UNSIGNED_SHORT:
            case CBT_UNSIGNED_LONG:
            case CBT_UNSIGNED_LONG_LONG:
                return node->number.u == 0u;
            case CBT_FLOAT:
                return node->number.f == 0.0f;
            case CBT_DOUBLE:
                return node->number.d == 0.0;
            case CBT_LONG_DOUBLE:
                return node->number.ld == 0.0L;
        }
    }

    return false;
}

bool NodeIsNumberNot0(CConstNodePtr node) {
    assert(node != nullptr);

    if (node->type == CNT_NUMBER) {
        switch (node->ctype.GetAsmType()) {
            case CBT_CHAR:
            case CBT_SHORT:
            case CBT_LONG:
            case CBT_LONG_LONG:
                return node->number.i != 0;
            case CBT_UNSIGNED_CHAR:
            case CBT_UNSIGNED_SHORT:
            case CBT_UNSIGNED_LONG:
            case CBT_UNSIGNED_LONG_LONG:
                return node->number.u != 0u;
            case CBT_FLOAT:
                return node->number.f != 0.0f;
            case CBT_DOUBLE:
                return node->number.d != 0.0;
            case CBT_LONG_DOUBLE:
                return node->number.ld != 0.0L;
        }
    }

    return false;
}
