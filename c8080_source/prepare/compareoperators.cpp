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
#include "../c/tools/nodeisnumber.h"
bool PrepareCompareOperators(Prepare &, CNodePtr &node) {
    if (node->type != CNT_OPERATOR)
        return false;

    // Replace "cmp" with "or a"
    if (node->b->type == CNT_NUMBER && node->a->ctype.IsUnsigned()) {
        // Replace UNSIGNED <= 0 with UNSIGNED == 0
        if (node->operator_code == COP_CMP_LE && NodeIsNumber0(node->b)) {
            node->operator_code = COP_CMP_E;
            return true;
        }
        // Replace UNSIGNED > 0 with UNSIGNED != 0
        if (node->operator_code == COP_CMP_G && NodeIsNumber0(node->b)) {
            node->operator_code = COP_CMP_NE;
            return true;
        }
        // Replace UNSIGNED >= 1 with UNSIGNED != 0
        if (node->operator_code == COP_CMP_GE && NodeIsNumber1(node->b)) {
            node->operator_code = COP_CMP_NE;
            node->b->number.u = 0;
            return true;
        }
        // Replace UNSIGNED < 1 with UNSIGNED == 0
        if (node->operator_code == COP_CMP_L && NodeIsNumber1(node->b)) {
            node->operator_code = COP_CMP_E;
            node->b->number.u = 0;
            return true;
        }
    }

    return false;
}
