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

#include "InternalFunctions.h"
#include <stdexcept>

namespace I8080 {

void InternalFunctions::Init(CProgramm &p) {
    init = FindFunction(p, "__init");
    sub_16 = FindFunction(p, "__o_sub_16");
    mul_8 = FindFunction(p, "__o_mul_8");
    div_u8 = FindFunction(p, "__o_div_u8");
    div_i8 = FindFunction(p, "__o_div_i8");
    mod_u8 = FindFunction(p, "__o_mod_u8");
    mod_i8 = FindFunction(p, "__o_mod_i8");
    shl_8 = FindFunction(p, "__o_shl_8");
    shr_u8 = FindFunction(p, "__o_shr_u8");
    shr_i8 = FindFunction(p, "__o_shr_i8");
    and_16 = FindFunction(p, "__o_and_16");
    or_16 = FindFunction(p, "__o_or_16");
    xor_16 = FindFunction(p, "__o_xor_16");
    mul_16 = FindFunction(p, "__o_mul_16");
    div_u16 = FindFunction(p, "__o_div_u16");
    div_i16 = FindFunction(p, "__o_div_i16");
    mod_u16 = FindFunction(p, "__o_mod_u16");
    mod_i16 = FindFunction(p, "__o_mod_i16");
    shl_16 = FindFunction(p, "__o_shl_16");
    shl_16 = FindFunction(p, "__o_shl_16");
    shr_u16 = FindFunction(p, "__o_shr_u16");
    shr_i16 = FindFunction(p, "__o_shr_i16");
    add_32 = FindFunction(p, "__o_add_32");
    sub_32 = FindFunction(p, "__o_sub_32");
    minus_16 = FindFunction(p, "__o_minus_16");
    neg_16 = FindFunction(p, "__o_neg_16");
    call_hl = FindFunction(p, "__o_call_hl");
    i8_to_i16 = FindFunction(p, "__o_i8_to_i16");
    i16_to_i32 = FindFunction(p, "__o_i16_to_i32");
    load_32 = FindFunction(p, "__o_load_32");
    set_32 = FindFunction(p, "__o_set_32");
    and_32 = FindFunction(p, "__o_and_32");
    or_32 = FindFunction(p, "__o_or_32");
    xor_32 = FindFunction(p, "__o_xor_32");
    mul_32 = FindFunction(p, "__o_mul_32");
    div_u32 = FindFunction(p, "__o_div_u32");
    div_i32 = FindFunction(p, "__o_div_i32");
    mod_u32 = FindFunction(p, "__o_mod_u32");
    mod_i32 = FindFunction(p, "__o_mod_i32");
    shl_32 = FindFunction(p, "__o_shl_32");
    shr_u32 = FindFunction(p, "__o_shr_u32");
    shr_i32 = FindFunction(p, "__o_shr_i32");
    minus_32 = FindFunction(p, "__o_minus_32");
    neg_32 = FindFunction(p, "__o_neg_32");
    memcpy = FindFunction(p, "memcpy");
}

CVariablePtr InternalFunctions::FindFunction(CProgramm &p, CString name) {
    const CVariablePtr fn = p.FindVariable(name);
    if (fn == nullptr || !fn->type.IsFunction() || fn->only_extern)
        throw std::runtime_error("mandatory function " + name + " is not found");
    return fn;
}

}  // namespace I8080
