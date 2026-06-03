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

#include "index.h"
#include "../c/tools/makeoperator.h"

// Inside: if, while, do, !, &&, ||, ?:
// must be a condition or a numeric constant (true / false)

static bool PrepareJump2(CNodePtr &node) {
    if (node && !node->IsJumpNode() && node->type != CNT_NUMBER) {
        CNodePtr ch = CNODE({CNT_NUMBER, ctype : node->ctype, e : node->e});
        node = MakeOperator(COP_CMP_NE, node, ch, node->e, false);
        assert(node->IsJumpNode());
        return true;
    }
    return false;
}

bool PrepareJump(Prepare &, CNodePtr &parent) {
    switch (parent->type) {
        case CNT_OPERATOR:
            if (parent->operator_code == COP_IF)
                return PrepareJump2(parent->a);
            if (parent->operator_code == COP_LAND || parent->operator_code == COP_LOR) {
                const bool b = PrepareJump2(parent->a);
                const bool a = PrepareJump2(parent->b);
                return a || b;
            }
            return false;
        case CNT_MONO_OPERATOR:
            if (parent->mono_operator_code == MOP_NOT)
                return PrepareJump2(parent->a);
            return false;
        case CNT_IF:
        case CNT_WHILE:
        case CNT_DO:
            return PrepareJump2(parent->a);
        case CNT_FOR:
            return PrepareJump2(parent->b);
    }
    return false;
}
