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

#include "ccondcompilation.h"
#include "../cbasetype.h"

class CLex : public CCondCompilation {
public:
    bool IfToken(CToken t) {
        if (token != t)
            return false;
        NextToken();
        return true;
    }

    void NeedToken(CToken token) {
        if (!IfToken(token))
            ThrowSyntaxError();
    }

    bool WantToken(CToken token) {
        if (!IfToken(token)) {
            SyntaxError();
            return false;
        }
        return true;
    }

    bool IfInteger(uint64_t &out_number, CBaseType &out_type) {
        if (token != CT_INTEGER)
            return false;
        out_number = token_integer;
        out_type = CalculateIntegerType();
        NextToken();
        return true;
    }

    uint64_t NeedInteger(CBaseType &out_suffix) {
        uint64_t result = 0;
        if (!IfInteger(result, out_suffix))
            ThrowSyntaxError();
        return result;
    }

    bool WantInteger(uint64_t &out_number, CBaseType &out_type) {
        if (!IfInteger(out_number, out_type)) {
            SyntaxError();
            return false;
        }
        return true;
    }

    bool IfToken(const char *string) {
        if (0 != strncmp(token_data, string, token_size) || string[token_size] != 0)
            return false;
        NextToken();
        return true;
    }

    bool IfFloat(long double &out_number, CBaseType &out_type) {
        if (token != CT_FLOAT)
            return false;
        out_number = token_float;
        out_type = ParseFloatSuffix();
        NextToken();
        return true;
    }

    void NeedToken(const char *string) {
        if (!IfToken(string))
            ThrowSyntaxError();
    }

    bool WantToken(const char *string) {
        if (!IfToken(string)) {
            SyntaxError();
            return false;
        }
        return true;
    }

    bool CloseToken(const char *string, const char *close) {
        if (IfToken(string))
            return true;
        SyntaxError();
        do {
            NextToken();
        } while (!IfToken(close) && !IfToken(CT_EOF));
        return false;
    }

    bool IfToken(const std::string &string) {
        if (token_size != string.size() || 0 != memcmp(token_data, string.data(), token_size))
            return false;
        NextToken();
        return true;
    }

    void NeedToken(const std::string &string) {
        if (!IfToken(string))
            ThrowSyntaxError();
    }

    bool IfString1(std::string &out_string, std::map<uint32_t, uint8_t> *codepage = nullptr);

    void NeedString1(std::string &out_string) {
        if (!IfString1(out_string))
            ThrowSyntaxError();
    }

    bool WantString1(std::string &string) {
        if (!IfString1(string)) {
            SyntaxError();
            return false;
        }
        return true;
    }

    bool IfString2(std::string &out_string, std::map<uint32_t, uint8_t> *codepage = nullptr);

    void NeedString2(std::string &out_string) {
        if (!IfString2(out_string))
            ThrowSyntaxError();
    }

    bool IfIdent(std::string &out_string) {
        if (token != CT_IDENT)
            return false;
        out_string.assign(token_data, token_size);
        NextToken();
        while (token_size == 2 && token_data[0] == '#' && token_data[1] == '#') {
            NextToken();
            out_string.append(token_data, token_size);
            NextToken();
        }
        return true;
    }

    void NeedIdent(std::string &out_string) {
        if (!IfIdent(out_string))
            ThrowSyntaxError();
    }

    bool WantIdent(std::string &out_string) {
        if (IfIdent(out_string))
            return true;
        SyntaxError();
        return false;
    }

    bool IfToken(const std::vector<std::string> &strings, size_t &out_index);

    size_t NeedToken(const std::vector<std::string> &strings) {
        size_t index = 0;
        if (!IfToken(strings, index))
            ThrowSyntaxError();
        return index;
    }

    bool IfToken(const char *const *strings, size_t &out_index);

    void NeedToken(const char **strings, size_t &out_index) {
        if (!IfToken(strings, out_index))
            ThrowSyntaxError();
    }

    template <class T>
    bool IfToken(const std::vector<T> &array, size_t &out_index) {
        for (auto i = array.rbegin(); i != array.rend(); i++) {
            if (IfToken(i->name)) {
                out_index = array.size() - 1 - (i - array.rbegin());
                return true;
            }
        }
        return false;
    }

    template <class T>
    bool IfTokenP(const std::vector<T> &array, size_t &out_index) {
        for (auto i = array.rbegin(); i != array.rend(); i++) {
            if (IfToken((*i)->name)) {
                out_index = array.size() - 1 - (i - array.rbegin());
                return true;
            }
        }
        return false;
    }

private:
    unsigned ParseIntegerSuffix();
    CBaseType CalculateIntegerType();
    CBaseType ParseFloatSuffix();
};
