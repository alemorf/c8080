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

bool PrepareRemoveUselessOperations(Prepare &p, CNodePtr &node) {
    if (node->type == CNT_OPERATOR) {
        switch (node->operator_code) {
            case COP_SHL:
                if (NodeIsNumber0(node->b))  // Replace X << 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SHR:
                if (NodeIsNumber0(node->b))  // Replace X >> 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_OR:
                if (NodeIsNumber0(node->b))  // Replace X | 0 with X
                    return DeleteNodeSaveType(node, 'a');
                if (NodeIsNumber0(node->a))  // Replace 0 | X with X
                    return DeleteNodeSaveType(node, 'b');
                // TODO: 0xFFFF
                return false;
            case COP_XOR:
                if (NodeIsNumber0(node->b))  // Replace X ^ 0 with X
                    return DeleteNodeSaveType(node, 'a');
                if (NodeIsNumber0(node->a))  // Replace 0 ^ X with X
                    return DeleteNodeSaveType(node, 'b');
                return false;
            case COP_ADD:
                if (NodeIsNumber0(node->b))  // Replace X + 0 with X
                    return DeleteNodeSaveType(node, 'a');
                if (NodeIsNumber0(node->a))  // Replace 0 + X with X
                    return DeleteNodeSaveType(node, 'b');
                return false;
            case COP_SUB:
                if (NodeIsNumber0(node->b))  // Replace X - 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_MUL:
                if (NodeIsNumber0(node->a)) {
                    std::swap(node->a, node->b);
                    node->operator_code = COP_COMMA;
                    return true;
                }
                if (NodeIsNumber0(node->b)) {
                    node->operator_code = COP_COMMA;
                    return true;
                }
                if (NodeIsNumber1(node->b))  // Replace X * 1 with X
                    return DeleteNodeSaveType(node, 'a');
                if (NodeIsNumber1(node->a))  // Replace 1 * X with X
                    return DeleteNodeSaveType(node, 'b');
                // TODO: 0
                return false;
            case COP_DIV:
                if (NodeIsNumber1(node->b))  // Replace X / 1 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_LAND:
                if (NodeIsNumberNot0(node->b))  // Replace X && true with X
                    return DeleteNodeSaveType(node, 'a');
                if (NodeIsNumberNot0(node->a))  // Replace true && X with X
                    return DeleteNodeSaveType(node, 'b');
                // TODO: false
                return false;
            case COP_LOR:
                if (NodeIsNumber0(node->b))  // Replace X || 0 with X
                    return DeleteNodeSaveType(node, 'a');
                if (NodeIsNumber0(node->a))  // Replace 0 || X with X
                    return DeleteNodeSaveType(node, 'b');
                // TODO: true
                return false;
            case COP_SET_ADD:
                if (NodeIsNumber0(node->b))  // Replace X += 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SET_SUB:
                if (NodeIsNumber0(node->b))  // Replace X -= 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SET_MUL:              // TODO: Not work
                if (NodeIsNumber1(node->b))  // Replace X *= 1 with X
                    return DeleteNodeSaveType(node, 'a');
                // TODO: 0
                return false;
            case COP_SET_DIV:              // TODO: Not work
                if (NodeIsNumber1(node->b))  // Replace X /= 1 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SET_SHR:
                if (NodeIsNumber0(node->b))  // Replace X >>= 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SET_SHL:
                if (NodeIsNumber0(node->b))  // Replace X <<= 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
            case COP_SET_OR:
                if (NodeIsNumber0(node->b))  // Replace X |= 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
                // TODO: 0xFFFF
            case COP_SET_XOR:
                if (NodeIsNumber0(node->b))  // Replace X ^= 0 with X
                    return DeleteNodeSaveType(node, 'a');
                return false;
                // TODO: COP_CMP_L
                // TODO: COP_CMP_G
                // TODO: COP_CMP_LE
                // TODO: COP_CMP_GE
                // TODO: COP_CMP_E
                // TODO: COP_CMP_NE
                // TODO: COP_MOD
                // TODO: COP_AND
                // TODO: COP_SET_AND
        }
    }
    return false;
}
