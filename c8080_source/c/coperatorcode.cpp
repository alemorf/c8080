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

#include "coperatorcode.h"
#include <stdexcept>

bool IsSetOperator(COperatorCode code) {
    switch (code) {
        case COP_SET:
        case COP_SET_ADD:
        case COP_SET_SUB:
        case COP_SET_MUL:
        case COP_SET_DIV:
        case COP_SET_MOD:
        case COP_SET_SHR:
        case COP_SET_SHL:
        case COP_SET_AND:
        case COP_SET_OR:
        case COP_SET_XOR:
            return true;
        case COP_CMP_L:
        case COP_CMP_G:
        case COP_CMP_LE:
        case COP_CMP_GE:
        case COP_CMP_E:
        case COP_CMP_NE:
        case COP_ADD:
        case COP_SUB:
        case COP_MUL:
        case COP_DIV:
        case COP_MOD:
        case COP_SHR:
        case COP_SHL:
        case COP_AND:
        case COP_OR:
        case COP_XOR:
        case COP_LAND:
        case COP_LOR:
        case COP_IF:
        case COP_COMMA:
        case COP_CMP_L_ADD_CONST:
        case COP_CMP_GE_ADD_CONST:
            return false;
    }
    return false;
}

bool IsCompareOperator(COperatorCode code) {
    switch (code) {
        case COP_CMP_E:
        case COP_CMP_NE:
        case COP_CMP_LE:
        case COP_CMP_GE:
        case COP_CMP_L:
        case COP_CMP_G:
        case COP_CMP_L_ADD_CONST:
        case COP_CMP_GE_ADD_CONST:
            return true;
        case COP_ADD:
        case COP_SUB:
        case COP_MUL:
        case COP_DIV:
        case COP_MOD:
        case COP_SHR:
        case COP_SHL:
        case COP_AND:
        case COP_OR:
        case COP_XOR:
        case COP_LAND:
        case COP_LOR:
        case COP_SET:
        case COP_SET_ADD:
        case COP_SET_SUB:
        case COP_SET_MUL:
        case COP_SET_DIV:
        case COP_SET_MOD:
        case COP_SET_SHR:
        case COP_SET_SHL:
        case COP_SET_AND:
        case COP_SET_OR:
        case COP_SET_XOR:
        case COP_IF:
        case COP_COMMA:
            return false;
    }
    return false;
}

COperatorCode NegativeCompareOperator(COperatorCode code) {
    switch (code) {
        case COP_CMP_E:
            return COP_CMP_NE;
        case COP_CMP_NE:
            return COP_CMP_E;
        case COP_CMP_L:
            return COP_CMP_GE;
        case COP_CMP_G:
            return COP_CMP_LE;
        case COP_CMP_LE:
            return COP_CMP_G;
        case COP_CMP_GE:
            return COP_CMP_L;
        case COP_CMP_L_ADD_CONST:
            return COP_CMP_GE_ADD_CONST;
        case COP_CMP_GE_ADD_CONST:
            return COP_CMP_L_ADD_CONST;
    }
    throw std::runtime_error("Internal error " + std::to_string(code) + " in " + __PRETTY_FUNCTION__);
}

const char *ToString(COperatorCode code) {
    switch (code) {
        case COP_CMP_L:
        case COP_CMP_L_ADD_CONST:
            return "<";
        case COP_CMP_G:
            return ">";
        case COP_CMP_LE:
            return "<=";
        case COP_CMP_GE_ADD_CONST:
        case COP_CMP_GE:
            return ">=";
        case COP_CMP_E:
            return "==";
        case COP_CMP_NE:
            return "!=";
        case COP_ADD:
            return "+";
        case COP_SUB:
            return "-";
        case COP_MUL:
            return "*";
        case COP_DIV:
            return "/";
        case COP_MOD:
            return "%";
        case COP_SHR:
            return ">>";
        case COP_SHL:
            return "<<";
        case COP_AND:
            return "&";
        case COP_OR:
            return "|";
        case COP_XOR:
            return "^";
        case COP_LAND:
            return "&&";
        case COP_LOR:
            return "||";
        case COP_SET:
            return "=";
        case COP_SET_ADD:
            return "+=";
        case COP_SET_SUB:
            return "-=";
        case COP_SET_MUL:
            return "*=";
        case COP_SET_DIV:
            return "/=";
        case COP_SET_MOD:
            return "%=";
        case COP_SET_SHR:
            return ">>=";
        case COP_SET_SHL:
            return "<<=";
        case COP_SET_AND:
            return "&=";
        case COP_SET_OR:
            return "|=";
        case COP_SET_XOR:
            return "^=";
        case COP_IF:
            return "?:";
        case COP_COMMA:
            return ",";
    }
    throw std::runtime_error("Internal error " + std::to_string(code) + " in " + __PRETTY_FUNCTION__);
}

bool IsArgumentsRearrangeAllowed(COperatorCode code) {
    switch (code) {
        case COP_ADD:
        case COP_AND:
        case COP_OR:
        case COP_XOR:
        case COP_MUL:
        case COP_CMP_E:
        case COP_CMP_NE:
            return true;
    }
    return false;
}
