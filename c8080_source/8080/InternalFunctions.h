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

#pragma once

#include "../c/cprogramm.h"

namespace I8080 {

class InternalFunctions {
public:
    CVariablePtr init;
    CVariablePtr call_hl;

    // 8 bit
    CVariablePtr shl_8;   // 8 << 8
    CVariablePtr shr_u8;  // 8 >> 8
    CVariablePtr shr_i8;  // 8 >> 8 (signed)
    CVariablePtr mul_8;   // 8 * 8
    CVariablePtr div_u8;  // 8 / 8
    CVariablePtr div_i8;  // 8 / 8 (signed)
    CVariablePtr mod_u8;  // 8 % 8
    CVariablePtr mod_i8;  // 8 % 8 (signed)

    // 16 bit
    CVariablePtr i8_to_i16;  // (uint16_t)8
    CVariablePtr minus_16;   // -16
    CVariablePtr neg_16;     // ~16
    CVariablePtr and_16;     // 16 & 16
    CVariablePtr sub_16;     // 16 - 16
    CVariablePtr or_16;      // 16 | 16
    CVariablePtr xor_16;     // 16 ^ 16
    CVariablePtr shl_16;     // 16 << 16
    CVariablePtr shr_u16;    // 16 >> 16
    CVariablePtr shr_i16;    // 16 >> 16 (signed)
    CVariablePtr mul_16;     // 16 * 16
    CVariablePtr div_u16;    // 16 / 16
    CVariablePtr div_i16;    // 16 / 16 (signed)
    CVariablePtr mod_u16;    // 16 % 16
    CVariablePtr mod_i16;    // 16 % 16 (signed)

    // 32 bit
    CVariablePtr i16_to_i32;  // (uint32_t)16
    CVariablePtr load_32;     // 32 = *ptr32
    CVariablePtr set_32;      // *ptr32 = 32
    CVariablePtr minus_32;    // -32
    CVariablePtr neg_32;      // ~32
    CVariablePtr add_32;      // 32 + 32
    CVariablePtr sub_32;      // 32 - 32
    CVariablePtr or_32;       // 32 | 32
    CVariablePtr and_32;      // 32 & 32
    CVariablePtr xor_32;      // 32 ^ 32
    CVariablePtr shl_32;      // 32 << 32
    CVariablePtr shr_u32;     // 32 >> 32
    CVariablePtr shr_i32;     // 32 >> 32 (signed)
    CVariablePtr mul_32;      // 32 * 32
    CVariablePtr div_u32;     // 32 / 32
    CVariablePtr div_i32;     // 32 / 32 (signed)
    CVariablePtr mod_u32;     // 32 % 32
    CVariablePtr mod_i32;     // 32 % 32 (signed)

    // stdlib
    CVariablePtr memcpy;

    void Init(CProgramm &p);

protected:
    CVariablePtr FindFunction(CProgramm &p, CString name);
};

}  // namespace I8080
