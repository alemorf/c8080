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

#include "../../c/cprogramm.h"
#include "../../tools/strtouint64.h"

namespace I8080 {

static const enum AsmRegister REG_PORT = R32_DEHL;
static const enum AsmRegister REG_CONST = REG_PREPARE;

class CmmArg {
public:
    AsmRegister reg{REG_NONE};
    bool addr{};
    std::string text;

    bool Is0() const {
        return IsConst() && text == "0";
    }

    void SetReg(AsmRegister r) {
        addr = false;
        text.clear();
        reg = r;
    }

    bool IsReg(AsmRegister r) const {
        return !addr && reg == r;
    }

    bool IsRegAddr(AsmRegister r) const {
        return addr && reg == r;
    }

    bool IsA() const {
        return IsReg(R8_A);
    }

    bool IsHl() const {
        return IsReg(R16_HL);
    }

    bool Is8M() const {
        return !addr && IsAsmRegister8M(reg);
    }

    bool Is16Sp() {
        return !addr && IsAsmRegister16Sp(reg);
    }

    bool Is16Af() const {
        return !addr && IsAsmRegister16Af(reg);
    }

    void SetConst(CString text_) {
        assert(!text_.empty());
        addr = false;
        reg = REG_CONST;
        text = text_;
    }

    bool IsConst() const {
        return !addr && reg == REG_CONST;
    }

    bool IsConstNumber(uint64_t &out) const {
        return !addr && reg == REG_CONST && StrToUint64(text.c_str(), out);
    }

    bool IsConstAddr() const {
        return addr && reg == REG_CONST;
    }

    void SetPort(CString text_) {
        assert(!text_.empty());
        addr = false;
        reg = REG_PORT;
        text = text_;
    }

    bool IsPort() const {
        return !addr && reg == REG_PORT;
    }
};

std::string ToString(const CmmArg &arg);

}  // namespace I8080
