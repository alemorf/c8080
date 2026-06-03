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

namespace I8080 {

// i8080 have not "sub hl, de" instruction
// Replace "hl -= const" with "hl += -const"

bool PrepareSub16ToAdd16(Prepare &, CNodePtr &node) {
    if (node->type == CNT_OPERATOR && node->operator_code == COP_SUB) {
        if (node->b->type == CNT_NUMBER) {
            switch (node->ctype.GetAsmType()) {
                case CBT_INT16:
                    node->operator_code = COP_ADD;
                    node->b->number.i = int16_t(0 - node->b->number.i);
                    return true;
                case CBT_UINT16:
                    node->operator_code = COP_ADD;
                    node->b->number.u = uint16_t(0u - node->b->number.u);
                    return true;
            }
        } else if (node->b->type == CNT_CONST) {
            node->operator_code = COP_ADD;
            node->b->text = "0FFFFh & (0 - (" + node->b->text + "))";  // TODO: 0FFFF?
            return true;
        }
    }
    return false;
}

}  // namespace I8080
