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

#include "asmcondition.h"
#include <stdexcept>
#include <string>

namespace I8080 {

AsmCondition InvertAsmCondition(AsmCondition condition) {
    switch (condition) {
        case JC_Z:
            return JC_NZ;
        case JC_NZ:
            return JC_Z;
        case JC_C:
            return JC_NC;
        case JC_NC:
            return JC_C;
        case JC_P:
            return JC_M;
        case JC_M:
            return JC_P;
        case JC_PO:
            return JC_PE;
        case JC_PE:
            return JC_PO;
    }
    throw std::runtime_error("Incorrect AsmCondition " + std::to_string(condition));
}

const char *ToString(AsmCondition condition) {
    switch (condition) {
        case JC_Z:
            return "z";
        case JC_NZ:
            return "nz";
        case JC_C:
            return "c";
        case JC_NC:
            return "nc";
        case JC_P:
            return "p";
        case JC_M:
            return "m";
        case JC_PO:
            return "po";
        case JC_PE:
            return "pe";
    }
    return "?";
}

}  // namespace I8080
