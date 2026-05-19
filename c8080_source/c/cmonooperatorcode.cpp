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

#include "cmonooperatorcode.h"
#include <stdexcept>
#include <string>

const char *ToString(CMonoOperatorCode operator_code) {
    switch (operator_code) {
        case MOP_INC:
            return "++V";
        case MOP_DEC:
            return "--V";
        case MOP_PLUS:
            return "+";
        case MOP_MINUS:
            return "-";
        case MOP_NOT:
            return "!";
        case MOP_NEG:
            return "~";
        case MOP_DEADDR:
            return "*";
        case MOP_ADDR:
            return "&";
        case MOP_POST_INC:
            return "V++";
        case MOP_POST_DEC:
            return "V--";
        case MOP_ARRAY_ELEMENT:
            return "[]";
        case MOP_STRUCT_ITEM:
            return ".";
        case MOP_STRUCT_ITEM_POINTER:
            return "->";
    }
    throw std::runtime_error("Internal error, unsupported operator " + std::to_string(operator_code) + " in " +
                             __PRETTY_FUNCTION__);
}
