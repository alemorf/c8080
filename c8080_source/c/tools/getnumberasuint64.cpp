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

#include "getnumberasuint64.h"
#include "cthrow.h"

uint64_t GetNumberAsUint64(const CNodePtr &node) {
    assert(node != nullptr);

    if (node->type != CNT_NUMBER)
        C_ERROR_INTERNAL(node, "does not reduce to an integer constant");  // gcc

    switch (node->ctype.GetAsmType()) {
        case CBT_INT8:
        case CBT_INT16:
        case CBT_INT32:
        case CBT_INT64:
            return uint64_t(node->number.i);
        case CBT_UINT8:
        case CBT_UINT16:
        case CBT_UINT32:
        case CBT_UINT64:
            return node->number.u;
        case CBT_FLOAT:
            return uint64_t(node->number.f);
        case CBT_DOUBLE:
            return uint64_t(node->number.d);
        case CBT_LONG_DOUBLE:
            return uint64_t(node->number.ld);
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE(node);
    }

    return 0;
}
