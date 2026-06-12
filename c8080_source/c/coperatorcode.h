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

enum COperatorCode {
    COP_CMP_L,
    COP_CMP_G,
    COP_CMP_LE,
    COP_CMP_GE,
    COP_CMP_E,
    COP_CMP_NE,
    COP_ADD,
    COP_SUB,
    COP_MUL,
    COP_DIV,
    COP_MOD,
    COP_SHR,
    COP_SHL,
    COP_AND,
    COP_OR,
    COP_XOR,
    COP_LAND,
    COP_LOR,
    COP_SET,
    COP_SET_ADD,
    COP_SET_SUB,
    COP_SET_MUL,
    COP_SET_DIV,
    COP_SET_MOD,
    COP_SET_SHR,
    COP_SET_SHL,
    COP_SET_AND,
    COP_SET_OR,
    COP_SET_XOR,
    COP_IF,
    COP_COMMA,
    COP_CMP_L_ADD_CONST,  // Аналогичен COP_CMP_L, но один из аргументов негативный
                          // для использования быстрой команды процессора DAD
    COP_CMP_GE_ADD_CONST  // Аналогичен COP_CMP_GE, но один из аргументов негативный
                          // для использования быстрой команды процессора DAD
};

bool IsSetOperator(COperatorCode code);
bool IsCompareOperator(COperatorCode code);
COperatorCode NegativeCompareOperator(COperatorCode code);
const char *ToString(COperatorCode code);
bool IsArgumentsRearrangeAllowed(COperatorCode code);
