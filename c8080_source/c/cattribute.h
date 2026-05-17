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
#include <variant>
#include <stdint.h>
#include <assert.h>

// Example: void reboot(void) __address(0xF800);
// Example: extern int reboot __address("abc + 45");

struct CAddressAttribute {
    std::variant<std::monostate, uint64_t, std::string> value{};

    bool Exists() const {
        return !std::holds_alternative<std::monostate>(value);
    }

    std::string ToString() const {
        if (std::holds_alternative<std::string>(value))
            return std::get<std::string>(value);
        if (std::holds_alternative<uint64_t>(value))
            return std::to_string(std::get<uint64_t>(value));
        return "$notused$";
    }

    bool operator==(const CAddressAttribute &b) const {
        return value == b.value;
    }

    bool operator!=(const CAddressAttribute &b) const {
        return !(*this == b);
    }
};

// Example: __link("stdio/snprintf.c") int snprintf(char *buffer, size_t bufer_size, const char *format, ...);
// TODO: __include

struct CLinkAttribute {
    bool exists{};
    std::string base_name;
    const char *name_for_path;  // saved in programm objects

    bool operator==(const CLinkAttribute &b) const {
        return exists == b.exists && base_name == b.base_name;
        // name_for_path can contains path/folder or path\folder or PATH_FOLDER
    }

    bool operator!=(const CLinkAttribute &b) const {
        return !(*this == b);
    }
};

// Example: int data[256] __attribute__((aligned(8)));

struct CAlignAttribute {
    bool exists{};
    uint64_t value{};

    bool operator==(const CAlignAttribute &b) const {
        return exists == b.exists && value == b.value;
    }

    bool operator!=(const CAlignAttribute &b) const {
        return !(*this == b);
    }
};
