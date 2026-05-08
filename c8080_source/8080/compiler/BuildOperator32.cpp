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

#include "Compiler.h"

namespace I8080 {

void Compiler::BuildOperator32(CNodePtr &node) {
    //  TODO: Fast SHL, SHR, MUL, DIV
    Measure(node, R32_DEHL, &Compiler::Case_Operator32);
}

bool Compiler::Case_Operator32(CNodePtr &node, AsmRegister reg) {
    Build(node->a, R32_DEHL);
    out.push_de_hl();
    Build(node->b, R32_DEHL);
    switch (node->operator_code) {
        case COP_ADD:
            InternalCall(o.add_32);
            break;
        case COP_CMP_L:
        case COP_CMP_G:
        case COP_CMP_LE:
        case COP_CMP_GE:
        case COP_CMP_E:
        case COP_CMP_NE:
        case COP_SUB:
            InternalCall(o.sub_32);
            break;
        case COP_AND:
            InternalCall(o.and_32);
            break;
        case COP_OR:
            InternalCall(o.or_32);
            break;
        case COP_XOR:
            InternalCall(o.xor_32);
            break;
        case COP_MUL:
            InternalCall(o.mul_32);
            break;
        case COP_DIV:
            InternalCall(node->a->ctype.IsUnsigned() ? o.div_u32 : o.div_i32);
            break;
        case COP_MOD:
            InternalCall(node->a->ctype.IsUnsigned() ? o.mod_u32 : o.mod_i32);
            break;
        case COP_SHL:
            InternalCall(o.shl_32);
            break;
        case COP_SHR:
            InternalCall(node->a->ctype.IsUnsigned() ? o.shr_u32 : o.shr_i32);
            break;
        default:
            C_ERROR_UNSUPPORTED_OPERATOR(node);
    }
    out.stack_correction(-4);
    return true;
}

}  // namespace I8080
