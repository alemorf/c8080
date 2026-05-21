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

#include "clex.h"
#include "../tools/cdecodestring.h"
#include "../consts.h"

bool CLex::IfString1(std::string &out_string, std::map<uint32_t, uint8_t> *codepage) {
    if (token != CT_STRING1)
        return false;
    out_string.assign(token_data, token_size);
    const char *error = CDecodeString(out_string, codepage);
    if (error)
        Throw(error);
    NextToken();
    return true;
}

bool CLex::IfString2(std::string &out_string, std::map<uint32_t, uint8_t> *codepage) {
    if (token != CT_STRING2)
        return false;
    out_string.assign(token_data, token_size);
    NextToken();
    const char *error = CDecodeString(out_string, codepage);
    if (error)
        Throw(error);
    while (token == CT_STRING2) {
        std::string temp(token_data, token_size);
        NextToken();
        const char *error = CDecodeString(temp, codepage);
        if (error)
            Throw(error);
        out_string.append(temp);
    }
    return true;
}

bool CLex::IfToken(const char *const *strings, size_t &out_index) {
    for (auto i = strings; *i != nullptr; i++) {
        if (IfToken(*i)) {
            out_index = i - strings;
            return true;
        }
    }
    return false;
}

bool CLex::IfToken(const std::vector<std::string> &strings, size_t &out_index) {
    for (auto i = strings.begin(), i_end = strings.end(); i != i_end; i++) {
        if (IfToken(*i)) {
            out_index = i - strings.begin();
            return true;
        }
    }
    return false;
}

static const unsigned SUFFIX_L = 1;
static const unsigned SUFFIX_LL = 2;
static const unsigned SUFFIX_U = 1 << 8;

unsigned CLex::ParseIntegerSuffix() {
    size_t u_count = 0, l_count = 0;
    for (size_t i = 0; i < token_suffix_size; i++) {
        switch (token_suffix[i]) {
            case 'l':
            case 'L':
                l_count++;
                if (l_count <= 2)
                    continue;
                break;
            case 'u':
            case 'U':
                u_count++;
                if (u_count <= 1)
                    continue;
                break;
        }
        Error("invalid suffix \"" + std::string(token_suffix, token_suffix_size) + "\" on integer constant");  // gcc
        return 0;
    }
    return l_count | (u_count * SUFFIX_U);
}

CBaseType CLex::CalculateIntegerType() {
    if (token_number_errno == ERANGE)
        Warning("integer constant " + std::string(token_data, token_size) + " is too large");  // gcc

    const unsigned type = ParseIntegerSuffix();

    if (token_data[0] == '0') {  // octal and hexadecimal numbers
        if (token_integer > C_ULONG_MAX)
            return CBT_UNSIGNED_LONG_LONG;
        if (token_integer > C_LONG_MAX || (type & SUFFIX_LL))
            return (type & SUFFIX_U) ? CBT_UNSIGNED_LONG_LONG : CBT_LONG_LONG;
        if (token_integer > C_UINT_MAX || (type & SUFFIX_L))
            return (type & SUFFIX_U) ? CBT_UNSIGNED_LONG : CBT_LONG;
        if (token_integer > C_INT_MAX)
            return CBT_UNSIGNED_INT;
        return (type & SUFFIX_U) ? CBT_UNSIGNED_INT : CBT_INT;
    }

    if (type & SUFFIX_U) {
        if (token_integer > C_ULONG_MAX || (type & SUFFIX_LL))
            return CBT_UNSIGNED_LONG_LONG;
        if (token_integer > C_UINT_MAX || (type & SUFFIX_L))
            return CBT_UNSIGNED_LONG;
        return CBT_UNSIGNED_INT;
    }

    if (token_integer > C_ULONG_MAX) {
        Warning("integer constant is so large that it is unsigned");  // gcc
        return CBT_UNSIGNED_LONG_LONG;
    }
    if (token_integer > C_LONG_MAX || (type & SUFFIX_LL))
        return CBT_LONG_LONG;
    if (token_integer > C_INT_MAX || (type & SUFFIX_L))
        return CBT_LONG;
    return CBT_INT;
}

CBaseType CLex::ParseFloatSuffix() {
    if (token_number_errno == ERANGE)
        Warning("floating constant exceeds range (" + std::string(token_data, token_size) + ")");  // gcc

    if (token_suffix_size == 0)
        return CBT_DOUBLE;  // TODO: gcc show "warning: floating constant exceeds range of ‘double’" if 123e456
    if (token_suffix_size == 1) {
        if (token_suffix[0] == 'f' || token_suffix[0] == 'F')
            return CBT_FLOAT;
        if (token_suffix[0] == 'l' || token_suffix[0] == 'L')
            return CBT_LONG_DOUBLE;
    }

    Error("invalid suffix \"" + std::string(token_suffix, token_suffix_size) + "\" on floating constant");  // gcc
    return CBT_DOUBLE;
}
