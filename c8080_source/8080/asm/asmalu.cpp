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

#include "asmalu.h"
#include <array>
#include <stdexcept>
#include <string>

namespace I8080 {

const char *ToString(AsmAlu value) {
    static const char *strings[] = {"xor", "or", "and", "sub", "add", "cp", "adc", "sbc"};

    if (value < 0 || value >= std::size(strings))
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " " + std::to_string(value));

    return strings[value];
}

}  // namespace I8080
