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

#include "cparser.h"
#include "../../tools/cstring.h"
#include "../ctypedef.h"

class CParserBase {
public:
    CProgramm &programm;
    CParser &cparser;

    size_t current_stack_size{};
    size_t max_stack_size{};

    // View scope
    struct Level {
        size_t variables_count{};
        size_t structs_count{};
        size_t unions_count{};
        size_t typedefs_count{};
        size_t stack_size{};
    };

    std::vector<Level> prev_scopes;  // TODO: scoped_map
    std::vector<CStructPtr> scope_structs;
    std::vector<CStructPtr> scope_unions;
    std::vector<CTypedef> scope_typedefs;
    std::vector<CVariablePtr> scope_variables;
    std::map<CString, CVariablePtr> scope_labels;

    CParserBase(CParser &p) : programm(p.programm), cparser(p) {
    }
    void Utf8To8Bit(const CErrorPosition &e, CString in, std::string &out);
    void Enter();
    void Leave();
    CVariablePtr FindVariableCurrentScope(CString name);
    CTypedef *FindTypedefCurrentScope(CString name);
    CVariablePtr BindLabel(CString label_name, CErrorPosition &e, bool is_goto);
    CStructPtr BindStructUnion(CString name, bool is_union, bool global, CErrorPosition &e);
    CVariablePtr RegisterVariable(bool extern_flag, CNodePtr node, bool global, CString name);
    void RegisterTypedef(CNodePtr node, CString name);
};
