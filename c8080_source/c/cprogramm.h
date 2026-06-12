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
#include <list>
#include "cnode.h"
#include "cconststring.h"
#include "cvariable.h"
#include "../8080/treeextensions/cprogramm8080.h"

class CProgramm {
public:
    CNodePtr first_node;
    std::map<std::string, CConstStringPtr> const_strings;
    std::map<std::string, CStructPtr> structs;
    std::map<std::string, CStructPtr> unions;
    std::list<std::string> saved_strings;           // TODO: Remove
    std::map<std::string, CVariablePtr> variables;  // no static
    std::vector<CVariablePtr> all_top_variables;    // with static
    std::map<std::string, int> output_names;
    std::map<std::string, int> asm_names;
    bool cmm{};  // cmm language mode
    bool error{};
    uint64_t unique_counter{};
    std::string last_error;
    CVariableMode default_variables_mode = CVM_GLOBAL;

    CProgramm8080 c;

    bool AddOutputName(CString name);
    CVariablePtr FindVariable(CString name);
    void AddVariable(CVariablePtr a);
    CConstStringPtr RegisterConstString(CString text);
    void Error(const CErrorPosition &e, CString text, const char *type = "error");
    void Note(const CErrorPosition &e, CString text);
    const char *SaveString(const char *data, size_t size);
    const char *SaveString(const std::string &data) {
        return SaveString(data.c_str(), data.size());
    }

    CVariableMode GetVariableMode(CType &t) const {
        return t.variables_mode == CVM_NOT_SET ? default_variables_mode : t.variables_mode;
    }
};
