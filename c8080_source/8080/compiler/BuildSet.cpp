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

void Compiler::BuildSet(CNodePtr &node) {
    if (!node->a->IsDeaddr()) {
        p.Error(node->e, "lvalue required as left operand of assignment");  // gcc
        return;
    }

    switch (node->b->ctype.GetAsmType()) {
        case CBT_INT8:
        case CBT_UINT8:
            Measure(node, R8_A, &Compiler::Case_Set8_MM);      // MOV M, A
            Measure(node, R8_A, &Compiler::Case_Set8_AM);      // STAX D
            Measure(node, R8_A, &Compiler::Case_Set8_MA);      // MOV M, D
            Measure(node, R8_A, &Compiler::Case_Set8_MNR);     // STA const
            Measure(node, REG_NONE, &Compiler::Case_Set8_MN);  // MVI M, const
            break;
        case CBT_INT16:
        case CBT_UINT16:
            Measure(node, R16_HL, &Compiler::Case_Set16_AA);  // MOV M, E / INX H / MOV M, D
            Measure(node, R16_HL, &Compiler::Case_Set16_NM);  // SHLD const
            break;
        case CBT_INT32:
        case CBT_UINT32:
            Measure(node, R32_DEHL, &Compiler::Case_Set32_NM);  // SHLD const / XCHF / SHLD const + 2
            Measure(node, R32_DEHL, &Compiler::Case_Set32_MM);  // PUSH HL / CALL set_32
            Measure(node, R32_DEHL, &Compiler::Case_Set32_AM);  // PUSH DE / CALL set_32
            break;
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE(node->b);
    }
}

bool Compiler::Case_Set8_MM(CNodePtr &node, AsmRegister reg) {
    if (BuildDouble(node, reg, node->a->a, R16_HL, node->b, R8_A)) {
        out.ld_phl_a();
        return true;
    }
    return false;
}

bool Compiler::Case_Set8_AM(CNodePtr &node, AsmRegister reg) {
    if (BuildDouble(node, reg, node->a->a, R16_DE, node->b, R8_A)) {
        out.ld_pde_a();
        return true;
    }
    return false;
}

bool Compiler::Case_Set8_MA(CNodePtr &node, AsmRegister reg) {
    if (BuildDouble(node, reg, node->a->a, R16_HL, node->b, R8_D)) {
        out.ld_phl_d();
        MeasureResult(node, R8_D);
        MeasureResult(node, REG_NONE);
        if (reg == R8_A)
            out.ld_a_d();
        return true;
    }
    return false;
}

bool Compiler::Case_Set8_MN(CNodePtr &node, AsmRegister reg) {
    if (node->b->IsConstNode()) {
        Build(node->a->a, R16_HL);
        out.ld_phl_const(node->b);
        return true;
    }
    return false;
}

bool Compiler::Case_Set8_MNR(CNodePtr &node, AsmRegister reg) {
    if (node->a->a->IsConstNode()) {
        Build(node->b, R8_A);
        out.ld_pconst_a(node->a->a);
        return true;
    }
    return false;
}

bool Compiler::Case_Set16_AA(CNodePtr &node, AsmRegister reg) {
    BuildArgs2(node, reg, node->a->a, node->b, R16_HL, R16_DE, false);  // No swap
    out.ld_phl_e();
    out.inc_hl();
    out.ld_phl_d();
    MeasureResult(node, R16_DE);
    MeasureResult(node, REG_NONE);
    if (reg == R16_HL)
        out.ex_hl_de();
    return true;
}

bool Compiler::Case_Set16_NM(CNodePtr &node, AsmRegister reg) {
    if (node->a->a->IsConstNode()) {
        Build(node->b, R16_HL);
        out.ld_pconst_hl(node->a->a);
        return true;
    }
    return false;
}

bool Compiler::Case_Set32_MM(CNodePtr &node, AsmRegister reg) {
    Build(node->a->a, R16_HL);
    out.push_hl();
    Build(node->b, R32_DEHL);
    InternalCall(o.set_32);
    out.stack_correction(-2);
    return true;
}

bool Compiler::Case_Set32_AM(CNodePtr &node, AsmRegister reg) {
    if (node->a->a->compiler.alt.able) {
        Build(node->a->a, R16_DE);
        out.push_de();
        Build(node->b, R32_DEHL);
        InternalCall(o.set_32);
        out.stack_correction(-2);
        return true;
    }
    return false;
}

bool Compiler::Case_Set32_NM(CNodePtr &node, AsmRegister reg) {
    if (node->a->a->IsConstNode()) {
        Build(node->b, R32_DEHL);
        out.ld_pconst_dehl_xchg(node->a->a);
        MeasureResult(node, REG_NONE);
        if (reg != REG_NONE)
            out.ex_hl_de();
        return true;
    }
    return false;
}

}  // namespace I8080
