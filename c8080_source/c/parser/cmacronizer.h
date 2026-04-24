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

#include <map>
#include <functional>
#include <list>
#include <string>
#include <stdexcept>
#include <string.h>
#include <memory>
#include "../../tools/cstring.h"
#include "../cerrorposition.h"
#include "ctokenizer.h"

enum CMacroArgsMode { CMAM_FIXED, CMAM_VA_OPT, CMAM_VAR_LAST };

class CMacroizer : public CTokenizer {
protected:
    struct Macro {
        std::string name;
        const char *body{};
        bool disabled{};  // Macro should not call itself
        std::vector<std::string> args;
        std::shared_ptr<Macro> prev;
        CMacroArgsMode args_mode{CMAM_FIXED};
    };

    struct Stack {
        const char *cursor{};
        size_t line{};
        size_t column{};
        const char *file_name{};
        size_t endif_counter{};
        Macro *active_macro{};  // Macro should not call itself
    };

    std::map<CString, std::shared_ptr<Macro>> macro;
    std::list<Stack> stack;

    void Enter(Macro *macro_index, const char *contents, const char *name);
    void ReadDirective(std::string &result);
    bool Leave();

public:
    std::function<void(CErrorPosition &, CString)> on_error;
    std::function<const char *(const char *, size_t)> save_string;
    size_t endif_counter{};
    unsigned in_macro{};
    bool enable_macro_in_preprocessor{};
    CErrorPosition error_position;  // Use only if in macro > 0

    void Open(const char *contents, const char *file_name);
    void Include(const char *contents, const char *file_name);
    void AddMacro(CString name, const char *body = "", size_t size = 0, const std::vector<std::string> *args = nullptr,
                  CMacroArgsMode mode = CMAM_FIXED);
    bool FindMacro(CString name);
    bool DeleteMacro(CString name);
    void NextToken();
    void ThrowSyntaxError();
    void SyntaxError();
    void Throw(CString text);
    void Error(CString text);
    void Error(CString text, CErrorPosition &p);
    bool FindDirective(std::string &out);
    bool ReadRaw(std::string &result, char terminator1, char terminator2, char open);
};

inline CErrorPosition::CErrorPosition(const CMacroizer &lex) {
    if (lex.in_macro > 0) {
        *this = lex.error_position;
    } else {
        line = lex.token_line;
        column = lex.token_column;
        cursor = lex.token_data;
        file_name = lex.file_name;
    }
}
