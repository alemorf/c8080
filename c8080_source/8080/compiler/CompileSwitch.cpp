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
#include "../../c/tools/prepareswitch.h"
#include <limits.h>

namespace I8080 {

void Compiler::CompileSwitch(CNodePtr &node) {
    std::vector<CNodePtr> cases;
    if (!PrepareSwitch(p, node, cases))
        return;

    for (auto &i : cases)
        i->compiler.label = out.AllocLabel();

    AsmLabel *default_label;
    CNodePtr default_link = node->default_link.lock();
    if (default_link != nullptr) {
        default_label = out.AllocLabel();
        default_link->compiler.label = default_label;
    } else {
        default_label = break_label;
    }

    switch (node->a->ctype.GetAsmType()) {
        case CBT_INT8:
        case CBT_UINT8:
            CompileSwitch8(node, cases, default_label);
            break;
        case CBT_INT16:
        case CBT_UINT16:
            CompileSwitch16(node, cases, default_label);
            break;
        default:
            C_ERROR_INTERNAL(node->a, "no case");
    }
}

// TODO: Optimize. Value-pointer array. Binary search.

void Compiler::CompileSwitch8(CNodePtr &node, std::vector<CNodePtr> &cases, AsmLabel *default_label) {
    Build(node->a);
    Build(node->a, R8_A);

    uint8_t current = 0;
    for (auto &i : cases) {
        uint8_t v = uint8_t(i->number.u) - current;
        if (v == 0)
            out.alu_a_reg(ALU_OR, R8_A);
        else if (v == 1)
            out.dec_reg(R8_A);
        else
            out.alu_a_number(ALU_SUB, v);
        out.jz_label(i->compiler.label);
        current = uint8_t(i->number.u);
    }

    out.jmp_label(default_label);
}

void Compiler::CompileSwitch16(CNodePtr &node, std::vector<CNodePtr> &cases, AsmLabel *default_label) {
    Build(node->a);
    Build(node->a, R16_HL);

    uint16_t prev = 0;
    for (auto &i : cases) {
        uint16_t delta = uint16_t(0 - i->number.u);
        if (delta != 0) {
            out.ld_de_number(uint16_t(delta - prev));
            out.add_hl_de();
        }
        out.ld_r8_r8(R8_A, R8_L);
        out.alu_a_reg(ALU_OR, R8_H);
        out.jz_label(i->compiler.label);
        prev = delta;
    }

    out.jmp_label(default_label);
}

}  // namespace I8080
