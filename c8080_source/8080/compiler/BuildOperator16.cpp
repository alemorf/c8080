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
#include "../../c/tools/getnumberasuint64.h"

namespace I8080 {

void Compiler::BuildOperator16(CNodePtr &node) {
    node->compiler.rearrange = IsArgumentsRearrangeAllowed(node->operator_code);

    // First priority
    Measure(node, R16_HL, &Compiler::Case_Mul16_MC);
    Measure(node, R16_HL, &Compiler::Case_Mul16_MCR);
    Measure(node, R16_HL, &Compiler::Case_Mul16_AC);
    Measure(node, R16_HL, &Compiler::Case_Mul16_ACR);
    Measure(node, R16_HL, &Compiler::Case_Shl16_MN);
    Measure(node, R16_HL, &Compiler::Case_Shr16_MN);
    Measure(node, R16_HL, &Compiler::Case_IncDec16_N);
    Measure(node, R16_DE, &Compiler::Case_IncDec16_N);

    // Last priority
    if (!node->compiler.main.able)
        Measure(node, R16_HL, &Compiler::Case_Operator16);
}

void Compiler::Alu16(CNodePtr &node) {
    switch (node->operator_code) {
        case COP_ADD:
        case COP_CMP_L_ADD_CONST:
        case COP_CMP_GE_ADD_CONST:
            return out.add_hl_de();
        case COP_CMP_L:
        case COP_CMP_G:
        case COP_CMP_LE:
        case COP_CMP_GE:
        case COP_SUB:
            return InternalCall(o.sub_16);
        case COP_AND:
            return InternalCall(o.and_16);
        case COP_OR:
            return InternalCall(o.or_16);
        case COP_CMP_E:
        case COP_CMP_NE:
        case COP_XOR:
            return InternalCall(o.xor_16);
        case COP_MUL:
            return InternalCall(o.mul_16);
        case COP_DIV:
            return InternalCall(node->a->ctype.IsUnsigned() ? o.div_u16 : o.div_i16);
        case COP_MOD:
            return InternalCall(node->a->ctype.IsUnsigned() ? o.mod_u16 : o.mod_i16);
        case COP_SHL:
            return InternalCall(o.shl_16);
        case COP_SHR:
            return InternalCall(node->a->ctype.IsUnsigned() ? o.shr_u16 : o.shr_i16);
        default:
            C_ERROR_UNSUPPORTED_OPERATOR(node);
    }
}

bool Compiler::Case_Operator16(CNodePtr &node, AsmRegister reg) {
    BuildArgs2(node, reg, node->a, node->b, R16_HL, R16_DE, node->compiler.rearrange);
    Alu16(node);
    return true;
}

bool Compiler::Case_Shl16_MN(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_SHL && node->b->type == CNT_NUMBER) {
        const uint64_t value = GetNumberAsUint64(node->b);
        if (value >= 16u) {
            Build(node->a, REG_NONE);
            out.ld_hl_number(0);
        } else {
            OutMul16(node->a, 1u << value, R16_HL);
        }
        return true;
    }
    return false;
}

bool Compiler::Case_Shr16_MN(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_SHR && node->b->type == CNT_NUMBER && node->ctype.IsUnsigned()) {
        const uint64_t value = GetNumberAsUint64(node->b);
        if (value >= 16u) {
            Build(node->a, REG_NONE);
            out.ld_hl_number(0);
            return true;
        }
        if (value == 8u) {
            Build(node->a, R16_HL);
            out.ld_r8_r8(R8_L, R8_H);
            out.ld_r8_number(R8_H, 0);
            return true;
        }
        if (value == 0) {
            Build(node->a, reg);
            return true;
        }
    }
    return false;
}

bool Compiler::Case_IncDec16_N(CNodePtr &node, AsmRegister reg) {
    // COP_SUB replaced with COP_ADD
    if (node->operator_code == COP_ADD && (reg != R16_DE || node->compiler.alt.able)) {
        if (node->a->type == CNT_NUMBER)
            return OutIncDec16(reg, node->b, node->a);
        if (node->b->type == CNT_NUMBER)
            return OutIncDec16(reg, node->a, node->b);
    }
    return false;
}

bool Compiler::Case_Mul16_MC(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_MUL && node->b->type == CNT_NUMBER && node->a->ctype.IsUnsigned()) {
        OutMul16(node->a, GetNumberAsUint64(node->b), reg);
        return true;
    }
    return false;
}

bool Compiler::Case_Mul16_AC(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_MUL && node->b->type == CNT_NUMBER && node->a->compiler.alt.able &&
        node->a->ctype.IsUnsigned()) {
        OutMul16(node->a, GetNumberAsUint64(node->b), R16_DE);
        return true;
    }
    return false;
}

bool Compiler::Case_Mul16_MCR(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_MUL && node->a->type == CNT_NUMBER && node->a->ctype.IsUnsigned()) {
        OutMul16(node->b, GetNumberAsUint64(node->a), R16_HL);
        return true;
    }
    return false;
}

bool Compiler::Case_Mul16_ACR(CNodePtr &node, AsmRegister reg) {
    if (node->operator_code == COP_MUL && node->a->type == CNT_NUMBER && node->b->compiler.alt.able &&
        node->a->ctype.IsUnsigned()) {
        OutMul16(node->b, GetNumberAsUint64(node->a), R16_DE);
        return true;
    }
    return false;
}

}  // namespace I8080
