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

#include "ctype.h"
#include "cstruct.h"
#include "consts.h"
#include "tools/cthrow.h"
#include <stdexcept>

const CType CTYPE_SIZE{CBT_SIZE};
const CType CTYPE_UNSIGNED_CHAR{CBT_UNSIGNED_CHAR};
const CType CTYPE_SIGNED_CHAR{CBT_SIGNED_CHAR};
const CType CTYPE_FUNCTION{CBT_FUNCTION, function_args : {CStructItem{CBT_VOID}}};

bool CType::CompareNoStatic(const CType &b) const {
    if (base_type != b.base_type || flag_const != b.flag_const || flag_volatile != b.flag_volatile ||
        variables_mode != b.variables_mode || pointers != b.pointers)
        return false;

    switch (base_type) {
        case CBT_FUNCTION:
            if (function_args.size() != b.function_args.size() || many_function_args != b.many_function_args)
                return false;
            for (size_t i = 0; i < function_args.size(); i++)
                if (function_args[i].type != b.function_args[i].type)
                    return false;
            return true;
        case CBT_STRUCT:
            return struct_object == b.struct_object;
        default:
            return true;
    }
}

bool CType::operator==(const CType &b) const {
    if (!CompareNoStatic(b))
        return false;
    return flag_static == b.flag_static;
}

std::string CType::ToString() const {
    std::string result;
    if (flag_const)
        result += "const ";
    if (flag_volatile)
        result += "volatile ";

    if (variables_mode == CVM_GLOBAL)
        result += "__global ";
    else if (variables_mode == CVM_STACK)
        result += "__stack ";

    switch (base_type) {
        case CBT_STRUCT:
            if (struct_object != nullptr) {
                result += (struct_object->is_union ? "union " : "struct ");
                result += struct_object->name;
            } else {
                result += "struct ?";
            }
            break;
        case CBT_FUNCTION:
            assert(!function_args.empty());
            result += (function_args.empty() ? "?" : function_args.front().type.ToString());
            break;
        case CBT_VOID:
            result += "void";
            break;
        case CBT_CHAR:
            result += "char";
            break;
        case CBT_UNSIGNED_CHAR:
            result += "unsigned char";
            break;
        case CBT_SIGNED_CHAR:
            result += "signed char";
            break;
        case CBT_SHORT:
            result += "short";
            break;
        case CBT_UNSIGNED_SHORT:
            result += "unsigned short";
            break;
        case CBT_INT:
            result += "int";
            break;
        case CBT_UNSIGNED_INT:
            result += "unsigned int";
            break;
        case CBT_LONG:
            result += "long";
            break;
        case CBT_UNSIGNED_LONG:
            result += "unsigned long";
            break;
        case CBT_LONG_LONG:
            result += "long long";
            break;
        case CBT_UNSIGNED_LONG_LONG:
            result += "unsigned long long";
            break;
        case CBT_FLOAT:
            result += "float";
            break;
        case CBT_DOUBLE:
            result += "double";
            break;
        case CBT_LONG_DOUBLE:
            result += "long double";
            break;
        case CBT_VA_LIST:
            result += "va_list";
            break;
        default:
            assert(false);
            result += "?";
    }
    if (base_type == CBT_FUNCTION && !pointers.empty()) {
        result += "(";
    }
    for (auto &i : pointers) {
        if (!i.is_array) {
            result += "*";
        } else {
            result += "[";
            result += std::to_string(i.array_size);
            result += "]";
        }
        if (i.flag_const)
            result += " const";
        if (i.flag_volatile)
            result += " volatile";
        if (i.flag_restrict)
            result += " restict";
    }
    if (base_type == CBT_FUNCTION && !pointers.empty()) {
        result += ")";
    }
    if (base_type == CBT_FUNCTION) {
        result += "(";
        for (size_t i = 1; i < function_args.size(); i++) {
            if (i > 1)
                result += ", ";
            result += function_args[i].type.ToString();
            if (!function_args[i].name.empty()) {
                result += " ";
                result += function_args[i].name;
            }
        }
        if (many_function_args)
            result += "...";
        result += ")";
    }
    return result;
}

CBaseType CType::GetAsmType() const {
    if (pointers.size() != 0)
        return CBT_UNSIGNED_SHORT;

    switch (base_type) {
        case CBT_SIGNED_CHAR:
            return CBT_CHAR;
        case CBT_INT:
            return CBT_SHORT;
        case CBT_UNSIGNED_INT:
            return CBT_UNSIGNED_SHORT;
        default:
            return base_type;
    }
}

CBaseType CType::GetAsmTypeIgnoreSign() const {
    if (pointers.size() != 0)
        return CBT_UNSIGNED_SHORT;

    switch (base_type) {
        case CBT_CHAR:
        case CBT_SIGNED_CHAR:
            return CBT_UNSIGNED_CHAR;
        case CBT_SHORT:
        case CBT_INT:
        case CBT_UNSIGNED_INT:
            return CBT_UNSIGNED_SHORT;
        case CBT_LONG:
            return CBT_UNSIGNED_LONG;
        case CBT_LONG_LONG:
            return CBT_UNSIGNED_LONG_LONG;
        default:
            return base_type;
    }
}

uint64_t CType::SizeOfBase(const CErrorPosition &e) const {
    if (base_type == CBT_STRUCT) {
        if (struct_object == nullptr)
            CThrow(e, "Struct is not declared");
        if (!struct_object->inited)
            CThrow(e, "Struct " + struct_object->name + " is not declared");
        return struct_object->size_bytes;
    }
    return ::SizeOf(base_type, e);
}

uint64_t CType::SizeOf(const CErrorPosition &e) const {
    uint64_t total_size = 1;
    for (size_t i = pointers.size(); i != 0; i--) {
        if (!pointers[i - 1].is_array)
            return total_size * C_SIZEOF_POINTER;  // TODO: overflow
        total_size *= pointers[i - 1].array_size;  // TODO: overflow
    }
    return total_size == 0 ? 0 : (total_size * SizeOfBase(e));
}

uint64_t CType::SizeOfElement(const CErrorPosition &e) const {
    if (pointers.empty())
        return 1;  // Multiplier of a regular variable (unsigned i; i++;)
    uint64_t total_size = 1;
    for (size_t i = pointers.size() - 1; i != 0; i--) {
        if (!pointers[i - 1].is_array)
            return total_size * C_SIZEOF_POINTER;  // TODO: overflow
        total_size *= pointers[i - 1].array_size;  // TODO: overflow
    }
    return total_size == 0 ? 0 : (total_size * SizeOfBase(e));
}
