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

#include "cmacronizer.h"
#include <string>
#include <limits.h>
#include <list>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include "../../tools/listext.h"
#include "../tools/cthrow.h"

// TODO: a\EOLb это ab

void CMacroizer::Open(const char *contents, const char *file_name) {
    stack.clear();
    macro.clear();
    in_macro = 0;
    enable_macro_in_preprocessor = false;
    endif_counter = 0;
    error_position = CErrorPosition();
    macro_arg_level = 0;

    Open2(contents, file_name);
}

void CMacroizer::Include(const char *contents, const char *file_name) {
    assert(in_macro == 0);
    Enter(nullptr, contents, file_name);
}

void CMacroizer::ThrowSyntaxError() {
    Throw(std::string("syntax error, unexpected '") + std::string(token_data, token_size) + "'");
}

void CMacroizer::SyntaxError() {
    Error(std::string("syntax error, unexpected '") + std::string(token_data, token_size) + "'");
}

void CMacroizer::SyntaxError(const char *what) {
    Error(std::string("syntax error, expected '") + what + "'");
}

void CMacroizer::Throw(CString text) {
    CErrorPosition p(*this);
    CThrow(p, text);
}

void CMacroizer::Error(CString text) {
    CErrorPosition p(*this);
    Error(text, p);
}

void CMacroizer::Error(CString text, const CErrorPosition &p) {
    if (on_error)
        on_error(p, text, "error");
    else
        CThrow(p, text);
}

void CMacroizer::Warning(CString text) {
    CErrorPosition p(*this);
    Warning(text, p);
}

void CMacroizer::Warning(CString text, const CErrorPosition &p) {
    if (on_error)
        on_error(p, text, "warning");
    else
        CThrow(p, text);
}

void CMacroizer::NextToken() {
    NextToken0();

    if (token == CT_IDENT && cursor[0] == '#' && cursor[1] == '#') {
        temp.assign(token_data, token_size);
        do {
            NextToken0();
            if (token_size != 2 || token_data[0] != '#' || token_data[1] != '#')
                SyntaxError();
            NextToken0();
            temp.append(token_data, token_size);
        } while (cursor[0] == '#' && cursor[1] == '#');
        token = CT_IDENT;
        token_data = save_string(temp.c_str(), temp.size());
        token_size = temp.size();
    }
}

void CMacroizer::NextToken0() {
    for (;;) {
        NextToken2();

        if (token == CT_EOF) {
            if (Leave())
                continue;
            break;  // There are no more files
        }

        if (token == CT_REMARK || token == CT_EOL)
            continue;

        if (token == CT_IDENT) {
            auto mi = macro.find(CString(token_data, token_size));
            if (mi == macro.end())
                break;
            std::shared_ptr<Macro> mp = mi->second;
            while (mp->prev && mp->is_macro_arg != 0 && mp->is_macro_arg != macro_arg_level) {
                mp = mp->prev;
                if (mp == nullptr)
                    goto break2;
            }
            if (mp->disabled != 0 && mp->disabled_level != macro_arg_level)  // Macro should not call itself
                break;
            Macro &m = *mp;
            if (m.args_mode != CMAM_NONE) {
                if (cursor[0] != '(')
                    ThrowSyntaxError();
                NextToken();

                bool no_more_args = false;
                if (m.args.size() == 0) {
                    std::string arg_body;
                    no_more_args = ReadRaw(arg_body, '(', ')', '(');
                    if (!arg_body.empty())
                        no_more_args = false;
                } else {
                    bool last_arg_is_empty = false;
                    for (size_t j = 0; j < m.args.size(); j++) {
                        std::string arg_body;
                        const bool var_last = (m.args_mode != CMAM_FIXED && j + 1 == m.args.size());
                        if (!no_more_args) {
                            no_more_args = ReadRaw(arg_body, var_last ? '(' : ',', ')', '(');
                        } else {
                            last_arg_is_empty = true;
                            if (!var_last)
                                Error("not enough parameters in macro");
                        }
                        AddMacro(m.args[j], arg_body.c_str(), arg_body.size(), nullptr, CMAM_NONE, true);
                    }

                    if (m.args_mode == CMAM_VA_OPT) {
                        static const std::vector<std::string> args = {"__VA_OPT__"};
                        AddMacro("__VA_OPT__", "__VA_OPT__", last_arg_is_empty ? 0 : sizeof("__VA_OPT__") - 1, &args,
                                 CMAM_VAR_LAST, true);
                    }
                }
                if (!no_more_args) {
                    Error("extra parameters in macro");
                    std::string temp;
                    ReadRaw(temp, '(', ')', '(');
                }
            }
            m.disabled++;                        // Macro should not call itself
            m.disabled_level = macro_arg_level;  // For nested calls of the same macro
            Enter(&m, m.body, mi->first.c_str());
            if (m.is_macro_arg)
                macro_arg_level -= 2;
            continue;
        }

        break;
    }

    // Нужно выйти из параметра макроса, потому что вызывающая функция будет искать токен ##
break2:
    while (cursor[0] == 0)
        if (!Leave())
            break;
}

void CMacroizer::Enter(Macro *active_macro, const char *contents, const char *file_name_) {
    Stack *s = Add(stack);
    if (active_macro) {
        if (in_macro == 0) {
            error_position.line = token_line;
            error_position.column = token_column;
            error_position.cursor = token_data;
            error_position.file_name = file_name;
        }
        in_macro++;
    } else {
        if (in_macro != 0)
            Throw("can't include from macro");
    }
    s->column = column;
    s->line = line;
    s->cursor = cursor;
    s->file_name = file_name;
    s->endif_counter = endif_counter;
    s->active_macro = active_macro;
    s->macro_arg_level = macro_arg_level;
    file_name = file_name_;
    cursor = contents;
    line = 1;
    column = 1;
    endif_counter = 0;
    macro_arg_level++;
}

bool CMacroizer::Leave() {
    if (enable_macro_in_preprocessor && in_macro == 0)
        return false;

    if (endif_counter != 0)
        Error("unterminated #if");  // gcc

    if (in_macro > 0)
        in_macro--;

    if (stack.empty())
        return false;

    Stack &s = stack.back();

    if (s.active_macro) {
        assert(s.active_macro->disabled);
        if (s.active_macro->args_mode == CMAM_VA_OPT)
            DeleteMacro("__VA_OPT__");
        for (auto i : s.active_macro->args)
            DeleteMacro(i);
        s.active_macro->disabled--;
    }

    endif_counter = s.endif_counter;
    file_name = s.file_name;
    cursor = s.cursor;
    line = s.line;
    column = s.column;
    macro_arg_level = s.macro_arg_level;

    stack.pop_back();

    return true;
}

bool CMacroizer::FindDirective(std::string &result) {
    do {
        NextToken2();
        if (token == CT_EOF)
            return false;
    } while (token_data[0] != '#');
    ReadDirective(result);
    return true;
}

void CMacroizer::ReadDirective(std::string &result) {
    for (;;) {
        const char *start = cursor;
        NextToken2();
        if (token == CT_EOF || token == CT_EOL)
            break;
        if (token != CT_REMARK)
            result.append(token_data, cursor - start);
        else
            result.append(" ", 1);
    }

    // Trim end
    auto i = result.rbegin();
    while (i != result.rend() && (*i == '\r' || *i == '\t' || *i == ' '))
        i++;
    result.resize(result.size() - (i - result.rbegin()));
}

bool CMacroizer::ReadRaw(std::string &result, char terminator1, char terminator2, char open) {
    size_t level = 0;
    for (;;) {
        const char *start = cursor;
        NextToken2();
        if (token == CT_EOF)
            Throw(std::string("expected '") + terminator1 + "' or '" + terminator2 + "' at end of input");
        if (token_data[0] == '#')  // MACRO(name #endif), asm { #endif }
            Throw("сan't use # here");
        if (token_size == 1) {
            if (token_data[0] == open) {
                level++;
            } else if (token_data[0] == terminator2) {
                if (level == 0)
                    return true;
                level--;
            } else if (token_data[0] == terminator1 && level == 0) {
                return false;
            }
        }
        if (token != CT_REMARK)
            result.append(start, cursor - start);
    }
}

void CMacroizer::AddMacro(CString name, const char *body, size_t size, const std::vector<std::string> *args,
                          CMacroArgsMode mode, bool is_macro_arg) {
    // TODO: assert(!in_macro);
    std::shared_ptr<Macro> m = std::make_shared<Macro>();
    m->name = name;
    m->body = save_string(body, size);
    m->args_mode = mode;
    if (args != nullptr)
        m->args = *args;
    m->is_macro_arg = is_macro_arg ? (macro_arg_level + 1) : 0;
    auto &i = macro[m->name];
    m->prev = i;
    i = m;
}

bool CMacroizer::FindMacro(CString name) {
    return macro.find(name) != macro.end();
}

bool CMacroizer::DeleteMacro(CString name) {
    // TODO: assert(!in_macro);
    auto i = macro.find(name);
    if (i == macro.end())
        return false;

    if (i->second->prev)
        i->second = i->second->prev;
    else
        macro.erase(i);

    return true;
}
