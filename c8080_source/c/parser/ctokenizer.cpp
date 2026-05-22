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

#include "ctokenizer.h"
#include <errno.h>
#include <stdlib.h>
#include <assert.h>

static inline size_t Tab(size_t column) {
    static const size_t tab_size = 4;
    return ((column + tab_size) & ~tab_size) + 1;
}

void CTokenizer::Open2(const char *contents, const char *file_name_) {
    cursor = contents;
    file_name = file_name_;
    line = 1;
    column = 1;

    token = CT_EOF;
    token_integer = 0;
    token_float = 0;
    token_column = 0;
    token_line = 0;
    token_data = 0;
    token_size = 0;
    token_number_errno = 0;
    token_suffix = 0;
    token_suffix_size = 0;
}

void CTokenizer::NextToken2() {
    for (;;) {
        switch (*cursor) {
            case 0:
                break;
            case '\t':
                cursor++;
                column = Tab(column);
                continue;
            case ' ':
                cursor++;
                column++;
                continue;
            case '\r':
                cursor++;
                continue;
        }
        break;
    }

    token_line = line;
    token_column = column;
    token_data = cursor;

    token = NextToken3();

    token_size = cursor - token_data;

    for (const char *i = token_data; i < cursor; i++) {
        switch (*i) {
            case '\t':
                column = Tab(column);
                break;
            case '\r':
                break;
            case '\n':
                line++;
                column = 1;
                break;
            default:
                column++;
                break;
        }
    }
}

CToken CTokenizer::NextToken3() {
    char c = *cursor++;

    if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        for (;;) {
            c = *cursor;
            if (!(c == '_' || (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
                break;
            cursor++;
        }
        return CT_IDENT;
    }

    if (c >= '0' && c <= '9') {
        const char *start = cursor - 1;
        errno = 0;
        token_integer = strtoull(start, (char **)&cursor, 0);
        token_number_errno = errno;
        token_suffix = cursor;
        assert(start != cursor);
        if (cursor[0] == '.' || cursor[0] == 'e' || cursor[0] == 'E') {
            errno = 0;
            token_float = strtold(start, (char **)&cursor);
            token_number_errno = errno;
            token_suffix = cursor;
            assert(start != cursor);
            while (cursor[0] == '_' || (cursor[0] >= '0' && cursor[0] <= '9') ||
                   (cursor[0] >= 'a' && cursor[0] <= 'z') || (cursor[0] >= 'A' && cursor[0] <= 'Z'))
                cursor++;
            token_suffix_size = cursor - token_suffix;
            return CT_FLOAT;
        }
        while (cursor[0] == '_' || (cursor[0] >= '0' && cursor[0] <= '9') || (cursor[0] >= 'a' && cursor[0] <= 'z') ||
               (cursor[0] >= 'A' && cursor[0] <= 'Z'))
            cursor++;
        token_suffix_size = cursor - token_suffix;
        return CT_INTEGER;
    }

    switch (c) {
        case 0:
            cursor--;
            return CT_EOF;
        case '\n':
            return CT_EOL;
        case '\'':
            do {
                c = *cursor;
                if (c == 0 || c == '\n')
                    Throw("missing terminating \' character");  // gcc
                cursor++;
                if (c == '\\') {
                    char c1 = *cursor;
                    if (c1 == 0 || c1 == '\n')
                        Throw("missing terminating \' character");  // gcc
                    cursor++;
                }
            } while (c != '\'');
            return CT_STRING1;
        case '"':
            do {
                c = *cursor;
                if (c == 0 || c == '\n')
                    Throw("missing terminating \" character");  // gcc
                cursor++;
                if (c == '\\') {
                    char c1 = *cursor;
                    if (c1 == 0 || c1 == '\n')
                        Throw("missing terminating \" character");  // gcc
                    cursor++;
                }
            } while (c != '"');
            return CT_STRING2;
        case '!':
        case '=':
        case '%':
        case '^':
            if (*cursor == '=')  // %= ^= != ==
                cursor++;
            return CT_OPERATOR;
        case '/':
            switch (*cursor) {
                case '=':  // /=
                    cursor++;
                    return CT_OPERATOR;
                case '/':  // //
                    cursor++;
                    for (;;) {
                        c = *cursor;
                        if (c == 0 || (c == '\n' && cursor[-2] != '\\'))
                            break;
                        cursor++;
                    }
                    return CT_REMARK;
                case '*':  // /*
                    cursor++;
                    c = *cursor;
                    if (c == 0)
                        Throw("unterminated comment");  // gcc
                    cursor++;
                    for (;;) {
                        char c1 = *cursor;
                        if (c1 == 0)
                            Throw("unterminated comment");  // gcc
                        cursor++;
                        if (c == '*' && c1 == '/')
                            break;
                        c = c1;
                    }
                    return CT_REMARK;
            }
            return CT_OPERATOR;
        case '*':
            switch (*cursor) {
                case '/':  // */
                case '=':  // *=
                    cursor++;
            }
            return CT_OPERATOR;
        case '.':
            if (*cursor == '.' && cursor[1] == '.')  // ...
                cursor += 2;
            return CT_OPERATOR;
        case '+':
            switch (*cursor) {
                case '+':  // ++
                case '=':  // +=
                    cursor++;
            }
            return CT_OPERATOR;
        case '-':
            switch (*cursor) {
                case '-':  // --
                case '=':  // -=
                case '>':  // ->
                    cursor++;
            }
            return CT_OPERATOR;
        case '<':
            switch (*cursor) {
                case '=':
                    cursor++;  // <=
                    return CT_OPERATOR;
                case '<':
                    cursor++;  // <<
                    switch (*cursor) {
                        case '=':
                            cursor++;  // <<=
                    }
            }
            return CT_OPERATOR;
        case '>':
            switch (*cursor) {
                case '=':
                    cursor++;  // >=
                    return CT_OPERATOR;
                case '>':
                    cursor++;  // >>
                    switch (*cursor) {
                        case '=':
                            cursor++;  // >>=
                    }
            }
            return CT_OPERATOR;
        case '|':
            switch (*cursor) {
                case '|':  // ||
                case '=':  // |=
                    cursor++;
            }
            return CT_OPERATOR;
        case '&':
            switch (*cursor) {
                case '&':  // &&
                case '=':  // &=
                    cursor++;
            }
            return CT_OPERATOR;
        case '\\':
            switch (*cursor) {
                case '\n':
                    cursor++;
                    return CT_REMARK;
                case '\r':
                    if (cursor[1] == '\n') {
                        cursor += 2;
                        return CT_REMARK;
                    }
            }
            return CT_OPERATOR;
        case '#':
            switch (*cursor) {
                case '#':  // ##
                    cursor++;
            }
            return CT_OPERATOR;
        default:
            return CT_OPERATOR;
    }
}
