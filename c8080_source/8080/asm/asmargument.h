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

#include "asmregister.h"
#include <string>
#include <stddef.h>
#include <stdexcept>
#include <assert.h>
#include "asmlabel.h"
#include "../../tools/cstring.h"

namespace I8080 {

enum AsmArgumentType { AAT_NONE = 0, AAT_REG, AAT_STRING, AAT_NUMBER, AAT_LABEL };

struct AsmArgument {
    AsmArgumentType type{};
    AsmRegister reg{};
    uint16_t number{};
    std::string string;
    AsmLabel *label{};

    std::string ToString() {
        switch (type) {
            case AAT_NONE:
                return "";
            case AAT_STRING:
                return string;
            case AAT_NUMBER:
                return std::to_string(number);
            case AAT_LABEL:
                return std::to_string(label->number);
            default:
                throw std::runtime_error(__PRETTY_FUNCTION__);
        }
    }

    bool operator==(const AsmArgument &a) const {
        if (type != a.type)
            return false;
        switch (a.type) {
            case AAT_NONE:
                return true;
            case AAT_STRING:
                return string == a.string;
            case AAT_NUMBER:
                return number == a.number;
            case AAT_LABEL:
                return label == a.label;
            case AAT_REG:
                return reg == a.reg;
        }
        assert(false);
        return false;
    }

    bool operator!=(const AsmArgument &a) const {
        return !(*this == a);
    }

    void SetNone() {
        type = AAT_NONE;
    }

    void Set(AsmRegister r) {
        type = AAT_REG;
        reg = r;
    }

    void Set(CString s) {
        assert(!s.empty());
        type = AAT_STRING;
        string = s;
    }

    void Set(uint16_t n) {
        type = AAT_NUMBER;
        number = n;
    }

    void Set(AsmLabel *l) {
        type = AAT_LABEL;
        label = l;
        label->ref_count++;
    }

    bool Is0() const {
        switch (type) {
            case AAT_STRING:
                return string == "0";
            case AAT_NUMBER:
                return number == 0;
            default:
                return false;
        }
    }
};

}  // namespace I8080
