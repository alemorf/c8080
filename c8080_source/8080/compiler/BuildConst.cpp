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

bool Compiler::Case_Const0(CNodePtr &node, AsmRegister reg) {
    out.GetConst(node);  // ref counter
    return true;
}

bool Compiler::Case_Const8(CNodePtr &node, AsmRegister reg) {
    out.ld_r8_const(reg, node);
    return true;
}

bool Compiler::Case_Const16(CNodePtr &node, AsmRegister reg) {
    out.ld_r16_const(reg, node);
    return true;
}

bool Compiler::Case_Const32(CNodePtr &node, AsmRegister) {
    out.ld_dehl_const(node);
    return true;
}

void Compiler::BuildConst(CNodePtr &node) {
    assert(node->IsConstNode());

    Measure(node, REG_NONE, &Compiler::Case_Const0);

    switch (node->ctype.GetAsmType()) {
        case CBT_INT8:
        case CBT_UINT8:
            Measure(node, R8_A, &Compiler::Case_Const8);
            Measure(node, R8_D, &Compiler::Case_Const8);
            break;
        case CBT_INT16:
        case CBT_UINT16:
            Measure(node, R16_HL, &Compiler::Case_Const16);
            Measure(node, R16_DE, &Compiler::Case_Const16);
            break;
        case CBT_INT32:
        case CBT_UINT32:
            Measure(node, R32_DEHL, &Compiler::Case_Const32);
            break;
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE(node);
    }
}

}  // namespace I8080
