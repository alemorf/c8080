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

#include <vector>
#include <string>
#include <memory>
#include <assert.h>
#include "cbasetype.h"
#include "cpointer.h"
#include "cvariablemode.h"
#include "cerrorposition.h"

struct CStruct;
struct CStructItem;

class CType {
public:
    CBaseType base_type = CBT_VOID;
    bool flag_const = false;
    bool flag_volatile = false;
    bool flag_static = false;
    CVariableMode variables_mode = CVM_NOT_SET;
    std::vector<CPointer> pointers;
    std::vector<CStructItem> function_args;  // for CBT_FUNCTION
    bool many_function_args = false;         // for CBT_FUNCTION
    std::shared_ptr<CStruct> struct_object;  // for CBT_STRUCT

    std::string ToString() const;

    bool CompareNoStatic(const CType &b) const;

    bool operator==(const CType &b) const;

    bool operator!=(const CType &b) const {
        return !(*this == b);
    }

    bool IsConst() const {
        return pointers.empty() ? flag_const : pointers.back().flag_const;
    }

    bool IsSigned() const {
        return !pointers.empty() && (base_type == CBT_CHAR || base_type == CBT_SIGNED_CHAR || base_type == CBT_SHORT ||
                                     base_type == CBT_INT || base_type == CBT_LONG);
    }

    bool IsUnsigned() const {
        return !pointers.empty() || ::IsUnsigned(base_type);
    }

    bool IsConstPointer() const {
        if (pointers.empty()) {
            assert(false);
            return false;
        }
        return pointers.size() == 1 ? flag_const : pointers[pointers.size() - 1].flag_const;
    }

    bool IsVoidPointerIgnoreConst() const {
        return pointers.size() == 1 && base_type == CBT_VOID;
    }

    bool IsVoid() const {
        return pointers.size() == 0 && base_type == CBT_VOID;
    }

    bool IsFunction() const {
        return pointers.size() == 0 && base_type == CBT_FUNCTION;
    }

    bool IsFuncitonPointer() const {
        return pointers.size() == 1 && base_type == CBT_FUNCTION;
    }

    uint64_t SizeOf(const CErrorPosition &e) const;

    uint64_t SizeOfBase(const CErrorPosition &e) const;

    bool Is8BitType() const {
        return pointers.empty() &&
               (base_type == CBT_CHAR || base_type == CBT_SIGNED_CHAR || base_type == CBT_UNSIGNED_CHAR);
    }

    bool Is16BitType() const {
        return !pointers.empty() || (base_type == CBT_SHORT || base_type == CBT_UNSIGNED_SHORT ||
                                     base_type == CBT_INT || base_type == CBT_UNSIGNED_INT);
    }

    bool Is32BitType() const {
        return pointers.empty() && (base_type == CBT_LONG || base_type == CBT_UNSIGNED_LONG);
    }

    bool IsPointer() const {
        return !pointers.empty();
    }

    bool IsArray() const {
        return !pointers.empty() && pointers.back().is_array;
    }

    CBaseType GetAsmType() const;
    CBaseType GetAsmTypeIgnoreSign() const;

    uint64_t SizeOfElement(const CErrorPosition &e) const;
};

typedef const CType &CConstType;

const extern CType CTYPE_SIZE;
const extern CType CTYPE_UNSIGNED_CHAR;
const extern CType CTYPE_SIGNED_CHAR;
const extern CType CTYPE_FUNCTION;
