/*
 * c8080 compiler
 * Copyright (c) 2026 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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

#include "asmcommand.h"

namespace I8080 {

int GetAsmCommandSize(AssemblerCommand c) {
    switch (c) {
        case AC_MVI:
            return 2;
        case AC_LXI:
            return 3;
        case AC_LXI_STACK_ADDR:
            return 3;
        case AC_LXI_ARG_STACK_ADDR:
            return 3;
        case AC_LHLD:
            return 3;
        case AC_SHLD:
            return 3;
        case AC_STA:
            return 3;
        case AC_LDA:
            return 3;
        case AC_LDAX:
            return 1;
        case AC_STAX:
            return 1;
        case AC_MOV:
            return 1;
        case AC_XCHG:
            return 1;
        case AC_XTHL:
            return 1;
        case AC_RAL:
            return 1;
        case AC_RAR:
            return 1;
        case AC_RRC:
            return 1;
        case AC_RLC:
            return 1;
        case AC_JMP:
            return 3;
        case AC_JMP_CONDITION:
            return 3;
        case AC_PUSH:
            return 1;
        case AC_POP:
            return 1;
        case AC_RET:
            return 1;
        case AC_CALL:
            return 3;
        case AC_ALU_CONST:
            return 2;
        case AC_ALU_REG:
            return 1;
        case AC_DAD:
            return 1;
        case AC_SPHL:
            return 1;
        case AC_CMA:
            return 1;
        case AC_INC:  // INR, INX
            return 1;
        case AC_DEC:  // DCR, DCX
            return 1;
        case AC_CALL_CONDITION:
            return 3;
        case AC_RET_CONDITION:
            return 1;
        case AC_IN:
            return 2;
        case AC_OUT:
            return 2;
        case AC_PCHL:
            return 1;
        case AC_STC:
            return 1;
        case AC_EI:
            return 1;
        case AC_DI:
            return 1;
        case AC_DAA:
            return 1;
        case AC_NOP:
            return 1;
    }
    return 0;
}

}  // namespace I8080
