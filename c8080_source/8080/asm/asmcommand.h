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

namespace I8080 {

enum AssemblerCommand {
    AC_NOP,
    AC_REMOVED,
    AC_MVI,
    AC_LXI,
    AC_LHLD,
    AC_SHLD,
    AC_LDA,
    AC_STA,
    AC_LDAX,
    AC_STAX,
    AC_MOV,
    AC_XCHG,
    AC_XTHL,
    AC_RAL,
    AC_RAR,
    AC_RRC,
    AC_RLC,
    AC_JMP,
    AC_JMP_CONDITION,
    AC_PUSH,
    AC_POP,
    AC_RET,
    AC_RET_CONDITION,
    AC_CALL,
    AC_CALL_CONDITION,
    AC_ALU_CONST,
    AC_ALU_REG,
    AC_DAD,
    AC_SPHL,
    AC_CMA,
    AC_INC,
    AC_DEC,
    AC_IN,
    AC_OUT,
    AC_PCHL,
    AC_STC,
    AC_EI,
    AC_DI,
    AC_DAA,

    AC_LINE,
    AC_ASSEMBLER,
    AC_REMARK,
    AC_LABEL,
    AC_STACK_CORRECTION,
    AC_STACK_CORRECTION_RESET,
    AC_LXI_STACK_ADDR,
    AC_LXI_ARG_STACK_ADDR,
};

int GetAsmCommandSize(enum AssemblerCommand c);

}  // namespace I8080
