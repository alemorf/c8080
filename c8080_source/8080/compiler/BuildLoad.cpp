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

void Compiler::BuildLoad(CNodePtr &node) {
    assert(node->type == CNT_MONO_OPERATOR);
    assert(node->mono_operator_code == MOP_DEADDR);
    assert(node->a != nullptr);

    Measure(node, REG_NONE, &Compiler::Case_Direct);

    switch (node->ctype.GetAsmType()) {
        case CBT_INT8:
        case CBT_UINT8:
            Measure(node, R8_A, &Compiler::Case_LoadConstAddr_8);
            Measure(node, R8_A, &Compiler::Case_Load_8);
            Measure(node, R8_D, &Compiler::Case_Load_8);
            Measure(node, R8_A, &Compiler::Case_Load_8_A);
            break;
        case CBT_INT16:
        case CBT_UINT16:
            Measure(node, R16_HL, &Compiler::Case_LoadConstAddr_16);
            Measure(node, R16_HL, &Compiler::Case_Load_16);
            Measure(node, R16_DE, &Compiler::Case_Load_16);
            break;
        case CBT_INT32:
        case CBT_UINT32:
            Measure(node, R32_DEHL, &Compiler::Case_LoadConstAddr_32);
            Measure(node, R32_DEHL, &Compiler::Case_Load_32);
            break;
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE(node);
    }
}

bool Compiler::Case_LoadConstAddr_8(CNodePtr &node, AsmRegister reg) {
    assert(reg == R8_A);
    if (node->a->IsConstNode()) {
        out.ld_a_pconst(node->a);
        return true;
    }
    return false;
}

bool Compiler::Case_Load_8(CNodePtr &node, AsmRegister reg) {
    assert(reg == R8_A || reg == R8_D);
    Build(node->a, R16_HL);
    out.ld_r8_r8(reg, R8_M);
    return true;
}

bool Compiler::Case_Load_8_A(CNodePtr &node, AsmRegister reg) {
    assert(reg == R8_A);
    if (node->a->compiler.alt.able) {
        Build(node->a, R16_DE);
        out.ld_a_pde();
        return true;
    }
    return false;
}

bool Compiler::Case_LoadConstAddr_16(CNodePtr &node, AsmRegister reg) {
    assert(reg == R16_HL);
    if (node->a->IsConstNode()) {
        out.ld_hl_pconst(node->a);
        return true;
    }
    return false;
}

bool Compiler::Case_Load_16(CNodePtr &node, AsmRegister reg) {
    assert(reg == R16_HL || reg == R16_DE);
    Build(node->a, R16_HL);
    out.ld_e_phl();
    out.inc_hl();
    out.ld_d_phl();
    if (reg == R16_HL)
        out.ex_hl_de();
    return true;
}

bool Compiler::Case_LoadConstAddr_32(CNodePtr &node, AsmRegister reg) {
    assert(reg == R32_DEHL);
    if (node->a->IsConstNode()) {
        out.ld_dehl_pconst(node->a);
        return true;
    }
    return false;
}

bool Compiler::Case_Load_32(CNodePtr &node, AsmRegister reg) {
    assert(reg == R32_DEHL);
    Build(node->a, R16_HL);
    InternalCall(o.load_32);
    return true;
}

}  // namespace I8080
