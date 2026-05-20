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

#include <stdint.h>
#include "../../tools/cstring.h"

enum CToken {
    CT_EOF,       // Eof of file
    CT_EOL,       // Eof of line
    CT_IDENT,     // Abc_def
    CT_INTEGER,   // 0 123
    CT_FLOAT,     // 1.0 1.2e-2
    CT_OPERATOR,  // + - += <<= ...
    CT_STRING1,   // 'String'
    CT_STRING2,   // "String"
    CT_REMARK     // /* Comment * or // Comment
};

class CTokenizer {
public:
    const char *file_name{};
    const char *cursor{};
    size_t line{};
    size_t column{};

    CToken token{};
    uint64_t token_integer{};
    long double token_float{};
    size_t token_column{};
    size_t token_line{};
    int token_number_errno;
    const char *token_data{};
    size_t token_size{};
    const char *token_suffix{};
    size_t token_suffix_size{};

    void Open2(const char *contents, const char *file_name);
    void NextToken2();

    virtual void Throw(CString text) = 0;

private:
    CToken NextToken3();
};
