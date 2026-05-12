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

void Compiler::OutSubSpN(size_t value) {
    // value 1  = 5t 1b
    // value 2  = 10t 1b
    // value 3  = 15t 2b
    // value 4  = 20t 2b
    // value 5  = 25t 3b
    // value 6  = 30t 3b
    // value 7  = 35t 4b
    // value 8+ = 31t 5b / 39t 7b
    if (value <= 7u) {
        size_t i = value;
        while (i >= 2u) {
            out.push_reg(R16_BC);  // 1 byte, 10 ticks
            i -= 2u;
        }
        if (i == 1u)
            out.dec_reg(R16_SP);  // 1 byte, 5 ticks
    } else {
        out.ld_hl_number(0x10000u - value);  // 3 bytes, 16 ticks
        out.add_hl_sp();                     // 1 bytes, 10 ticks
        out.ld_sp_hl();                      // 1 bytes, 5 ticks
    }
}

void Compiler::OutAddSpN(AsmRegister save, size_t value) {
    // value 1  = 5t 1b
    // value 2  = 10t 1b
    // value 3  = 15t 2b
    // value 4  = 20t 2b
    // value 5  = 25t 3b
    // value 6  = 30t 3b
    // value 7  = 35t 4b
    // value 8+ = 31t 5b / 39t 7b
    if (value <= 7) {
        size_t i = value;
        while (i >= 2u) {
            out.pop_bc();  // 1 byte, 10 ticks
            i -= 2u;
        }
        if (i == 1u)
            out.inc_reg(R16_SP);  // 1 byte, 5 ticks
    } else {
        const bool need_ex = (save == R32_DEHL || save == R16_HL);
        if (need_ex)
            out.ex_hl_de();       // 1 byte, 4 ticks
        out.ld_hl_number(value);  // 3 bytes, 16 ticks
        out.add_hl_sp();          // 1 byte, 10 ticks
        out.ld_sp_hl();           // 1 byte, 5 ticks
        out.stack_correction(-value);
        if (need_ex)
            out.ex_hl_de();  // 1 byte, 4 ticks
    }
}

}  // namespace I8080
