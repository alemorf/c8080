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

#include "cstruct.h"
#include <stdexcept>

bool CStruct::operator==(const CStruct &b) const {
    if (name != b.name || items.size() != b.items.size() || is_union != b.is_union)
        return false;

    for (size_t i = 0; i < items.size(); i++)
        if (*(items[i]) != *(b.items[i]))
            return false;

    return true;
}

void CStruct::CalcOffsets(const CErrorPosition &e) {
    if (is_union) {
        uint64_t max_size = 0;
        for (auto &i : items) {
            i->struct_item_offset = 0;
            auto size = i->type.SizeOf(e);
            if (max_size < size)
                max_size = size;
        }
        size_bytes = max_size;
    } else {
        uint64_t offset = 0;
        for (auto &i : items) {
            i->struct_item_offset = offset;
            if (__builtin_add_overflow(offset, i->type.SizeOf(e), &offset))
                throw std::runtime_error("Address overflow when calculating offset");
        }
        size_bytes = offset;
    }
    inited = true;
}

CStructItemPtr CStruct::FindItem(CString name) {
    for (auto &i : items) {
        if (i->name.empty()) {
            if (i->type.pointers.empty() && i->type.base_type == CBT_STRUCT) {
                CStructPtr &s = i->type.struct_object;
                if (s != nullptr) {
                    CStructItemPtr result = s->FindItem(name);
                    if (result != nullptr)
                        return result;
                }
            }
        } else if (i->name == name) {
            return i;
        }
    }
    return nullptr;
}

std::string CStruct::ToString() const {
    std::string result = (is_union ? "union " : "struct ");
    if (!name.empty()) {
        result += name;
        result += " ";
    }
    result += "{ ";
    for (const auto &i : items) {
        result += i->ToString();
        result += "; ";
    }
    result += "}";
    return result;
}
