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

void Compiler::OutMul16(CNodePtr &var, uint16_t number, AsmRegister reg) {
    assert(var != nullptr);
    assert(var->ctype.Is16BitType());
    assert(reg == R16_HL || reg == R16_DE || reg == REG_NONE);

    if (reg == REG_NONE) {
        Build(var, REG_NONE);
        return;
    }

    if (number == 0) {
        Build(var, REG_NONE);
        out.ld_hl_number(0);
        return;
    }

    Build(var, reg);

    // Fast multiplication by 0x100
    if (number % 0x100 == 0) {
        number /= 0x100;
        out.ld_r8_r8(R8_H, reg == R16_DE ? R8_E : R8_L);
        out.ld_l_number(0);
        reg = R16_HL;
    }

    unsigned programm_size = 0;
    while (uint16_t(number >> programm_size) > 1)
        programm_size++;

    if (reg == R16_DE) {
        out.ld_r8_r8(R8_H, R8_D);
        out.ld_r8_r8(R8_L, R8_E);
    } else if ((number ^ (1 << programm_size)) != 0) {
        out.ld_r8_r8(R8_D, R8_H);
        out.ld_r8_r8(R8_E, R8_L);
    }

    for (unsigned i = programm_size; i > 0; i--) {
        out.add_hl_reg(R16_HL);
        if (number & (1 << (i - 1)))
            out.add_hl_reg(R16_DE);
    }
}

}  // namespace I8080
