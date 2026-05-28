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

#include "convert.h"
#include "nodeisnumber.h"
#include "../tools/cthrow.h"

// These types convert to each other without errors

static bool AllowedConversions(CConstType type) {
    if (type.IsPointer())
        return false;
    switch (type.base_type) {
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
        case CBT_SIGNED_CHAR:
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
        case CBT_INT:
        case CBT_UNSIGNED_INT:
        case CBT_LONG:
        case CBT_UNSIGNED_LONG:
        case CBT_LONG_LONG:
        case CBT_UNSIGNED_LONG_LONG:
        case CBT_FLOAT:
        case CBT_DOUBLE:
        case CBT_LONG_DOUBLE:
            return true;
    }
    return false;
}

static void ConvertCheck(CConstType to_type, CNodePtr from) {
    // When getting the value of a CONST variable, that CONST means nothing
    // const int a = 2;
    // int b = a;
    // int* const a = 2;
    // int* b = a;
    CType from_type = from->ctype;
    if (from_type.pointers.empty())
        from_type.flag_const = false;
    else
        from_type.pointers.back().flag_const = false;

    // void* -> any***
    if (from_type.IsVoidPointerIgnoreConst() && to_type.IsPointer() &&
        (!from_type.pointers[0].flag_const || to_type.pointers[0].flag_const))
        return;  // TODO: Experiment!

    // any*** -> void*
    if (from_type.IsPointer() && to_type.IsVoidPointerIgnoreConst() &&
        (!from_type.pointers[0].flag_const || to_type.pointers[0].flag_const))
        return;

    // 0 -> any***
    if (NodeIsNumber0(from) && to_type.pointers.size() != 0)
        return;

    // Simple types are converts any to any
    if (AllowedConversions(to_type) && AllowedConversions(from_type))
        return;

    // Forgot size of last array level
    if (from_type.pointers.size() > 0 && to_type.pointers.size() == from_type.pointers.size() &&
        !to_type.pointers.back().is_array && from_type.pointers.back().is_array) {
        bool need_drop = true;
        for (size_t i = 0u; i < (to_type.pointers.size() - 1u); i++) {
            if (from_type.pointers[i].is_array != to_type.pointers[i].is_array &&
                from_type.pointers[i].array_size != to_type.pointers[i].array_size) {
                need_drop = false;
                break;
            }
        }
        if (need_drop)
            from_type.pointers.back().ResetArray();
    }

    // Get const
    if (to_type.pointers.size() == from_type.pointers.size()) {
        for (size_t i = 0; i < to_type.pointers.size(); i++)
            if (to_type.pointers[i].flag_const)
                from_type.pointers[i].flag_const = true;
        if (to_type.flag_const)
            from_type.flag_const = true;
    }

    // Done
    if (from_type.CompareNoStatic(to_type))
        return;

    CThrow(from, "Can't convert from " + from->ctype.ToString() + " to " + to_type.ToString());
}

CNodePtr Convert(CConstType to_type, CNodePtr from, bool cmm) {
    assert(from != nullptr);
    if (!cmm)
        ConvertCheck(to_type, from);
    if (from->ctype.CompareNoStatic(to_type))
        return from;
    return CNODE({CNT_CONVERT, a : from, ctype : to_type, e : from->e});
}
