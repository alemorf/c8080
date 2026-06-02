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

AsmRegister Compiler::GetResultReg(const CType &type, bool alt, bool no_result, const CNodePtr &e) {
    if (no_result)
        return REG_NONE;
    switch (type.GetAsmType()) {
        case CBT_VOID:
            return REG_NONE;
        case CBT_INT8:
        case CBT_UINT8:
            return alt ? R8_D : R8_A;
        case CBT_INT16:
        case CBT_UINT16:
            return alt ? R16_DE : R16_HL;
        case CBT_INT32:
        case CBT_UINT32:
            if (alt)
                C_ERROR_INTERNAL(e, "no alternative register for 32-bit type");
            return R32_DEHL;
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE_INT(e, type.GetAsmType());
            return REG_NONE;
    }
}

}  // namespace I8080
