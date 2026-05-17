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

#include <string>
#include <vector>
#include <memory>
#include "cstructitem.h"
#include "cerrorposition.h"

struct CStruct {
    std::string name;
    std::vector<CStructItemPtr> items;
    uint64_t size_bytes{};
    bool inited{};
    bool is_union{};
    CErrorPosition e;

    bool operator==(const CStruct &b) const;

    bool operator!=(const CStruct &b) const {
        return !(*this == b);
    }

    std::string ToString() const;
    void CalcOffsets(const CErrorPosition &e);
    CStructItemPtr FindItem(CString name);
};

typedef std::shared_ptr<CStruct> CStructPtr;
