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

#include "index.h"
#include "../c/tools/makecnode.h"

// Replace
//   a = b;  // struct my a, b;
// by
//   *(struct my*)memcpy(&a, &b, sizeof(struct my));

// The C parser stores the access to array element (X[Y]) as:
//  ARRAY_ELEMENT(X, Y)
//  Will be replaced with:
//  1) MONOOPERATOR.DEADDR(OPERATOR.ADD(X, OPERATOR.MUL(SIZEOF, Y))) if struct_item is not array
//  2) OPERATOR.ADD(X, OPERATOR.MUL(SIZEOF, Y)) if struct_item is array

bool PrepareCopyStruct(Prepare &p, CNodePtr &node) {
    if (node->type == CNT_OPERATOR && node->operator_code == COP_SET && node->a->ctype.pointers.empty() &&
        node->a->ctype.base_type == CBT_STRUCT) {
        if (!p.memcpy) {
            p.memcpy = p.programm.FindVariable("memcpy");
            if (!p.memcpy) {
                p.programm.Error(node->e, "mandatory function memcpy is not found");
                return false;
            }
        }

        const size_t item_sizeof = node->a->ctype.SizeOf(node->a->e);

        node->type = CNT_FUNCTION_CALL;
        node->variable = p.memcpy;
        node->a = MakeCNodeAddr(node->a);
        node->ctype = node->a->ctype;
        node->b = MakeCNodeAddr(node->b);
        node->a->next_node = node->b;
        node->b = nullptr;
        node->a->next_node->next_node = MakeCNodeNumberSizeT(item_sizeof, node->e);
        node = MakeCNodeDeaddr(node);
    }
    return false;
}
