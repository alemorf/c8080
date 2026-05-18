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

#include "cparserbase.h"
#include <stdexcept>
#include "../../tools/utf8to8bit.h"

void CParserBase::Enter() {
    prev_scopes.push_back(Level{scope_variables.size(), scope_structs.size(), scope_unions.size(),
                                scope_typedefs.size(), current_stack_size});
}

void CParserBase::Leave() {
    if (prev_scopes.empty())  // TODO: Make special object scoped_map
        throw std::runtime_error("Internal error in " + std::string(__PRETTY_FUNCTION__));
    Level &level = prev_scopes.back();
    scope_variables.resize(level.variables_count);
    scope_structs.resize(level.structs_count);
    scope_unions.resize(level.unions_count);
    scope_typedefs.resize(level.typedefs_count);
    current_stack_size = level.stack_size;
    prev_scopes.pop_back();
}

CTypedef *CParserBase::FindTypedefCurrentScope(CString name) {
    const size_t start = prev_scopes.empty() ? 0 : prev_scopes.back().typedefs_count;
    for (size_t i = start; i < scope_typedefs.size(); i++)  // TODO: Use map
        if (scope_typedefs[i].name == name)
            return &scope_typedefs[i];
    return nullptr;
}

CVariablePtr CParserBase::FindVariableCurrentScope(CString name) {
    const size_t start = prev_scopes.empty() ? 0 : prev_scopes.back().variables_count;
    for (size_t i = start; i < scope_variables.size(); i++)  // TODO: Use map
        if (scope_variables[i]->name == name)
            return scope_variables[i];
    return nullptr;
}

CStructPtr CParserBase::BindStructUnion(CString name, bool is_union, bool is_global, CErrorPosition &e) {
    auto &scope = is_union ? scope_unions : scope_structs;
    auto &global_map = is_union ? programm.unions : programm.structs;

    if (!name.empty()) {
        // Check unique name in view scope
        for (auto i = scope.rbegin(); i != scope.rend(); i++)  // TODO: Use map
            if ((*i)->name == name)
                return (*i);

        // To check compatibility of types in different units
        if (is_global) {
            auto i = global_map.find(name);
            if (i != global_map.end())
                return i->second;
        }
    }

    // Allocate struct
    CStructPtr s = std::make_shared<CStruct>();
    s->e = e;
    s->is_union = is_union;

    if (!name.empty()) {
        s->name = name;

        scope.push_back(s);

        // To check compatibility of types in different units
        if (is_global)
            global_map[name] = s;
    }

    return s;
}

void CParserBase::Utf8To8Bit(const CErrorPosition &e, CString in, std::string &out) {
    size_t pos = ::Utf8To8Bit(cparser.codepage, in, out);
    if (pos != SIZE_MAX)
        programm.Error(e, "unsupported symbol at position " + std::to_string(pos) + " in string \"" + in + "\"");
}

CVariablePtr CParserBase::BindLabel(CString name, CErrorPosition &e, bool is_goto) {
    CVariablePtr &label = scope_labels[name];
    if (!label) {
        label = std::make_shared<CVariable>();
        label->name = name;
        label->only_extern = true;
        label->is_label = true;
        label->output_name = name;
        label->e = e;
        programm.AddVariable(label);
    }
    if (is_goto) {
        label->label_call_count++;
    } else {
        if (!label->only_extern)
            programm.Error(e, "duplicate label '" + name + "'");  // gcc
        label->only_extern = false;
    }
    return label;
}

void CParserBase::RegisterTypedef(CNodePtr node, CString name) {
    CTypedef *b = FindTypedefCurrentScope(name);
    if (b != nullptr) {
        if (b->type != node->ctype) {
            programm.Error(node->e,
                           "conflicting types for '" + name + "'; have '" + node->ctype.ToString() + "'");  // gcc
            programm.Note(b->e,
                          "previous declaration of '" + name + "' with type '" + b->type.ToString() + "'");  // gcc
        }
    } else {
        scope_typedefs.push_back(CTypedef{node->ctype, name, e : node->e});
    }
}

CVariablePtr CParserBase::RegisterVariable(bool extern_flag, CNodePtr node, bool global, CString name) {
    CVariablePtr v = FindVariableCurrentScope(name);
    if (v != nullptr) {
        if (node->ctype.variables_mode != CVM_NOT_SET && v->type.variables_mode == CVM_NOT_SET)
            v->type.variables_mode = node->ctype.variables_mode;
        if (node->ctype.variables_mode == CVM_NOT_SET && v->type.variables_mode != CVM_NOT_SET)
            node->ctype.variables_mode = v->type.variables_mode;
        if (v->type != node->ctype) {
            programm.Error(node->e,
                           "conflicting types for '" + name + "'; have '" + node->ctype.ToString() + "'");  // gcc
            programm.Note(v->e,
                          "previous declaration of '" + name + "' with type '" + v->type.ToString() + "'");  // gcc
        }
        if (!extern_flag && (!v->only_extern || v->address_attribute.Exists())) {
            programm.Error(node->e, "redefinition of '" + name + "'");     // gcc
            programm.Note(v->e, "previous definition of '" + name + "'");  // gcc
        }
        if (!extern_flag)
            v->only_extern = false;
        // TODO: void test() { extern int a; int a; }
        return v;
    }

    if (extern_flag)
        global = true;

    if (global && !node->ctype.flag_static) {
        v = programm.variables[name];
        if (v != nullptr) {
            if (node->ctype != v->type) {
                programm.Error(node->e,
                               "conflicting types for '" + name + "'; have '" + node->ctype.ToString() + "'");  // gcc
                programm.Note(v->e,
                              "previous declaration of '" + name + "' with type '" + v->type.ToString() + "'");  // gcc
            }
            if (!extern_flag && !v->only_extern) {
                programm.Error(node->e, "redefinition of '" + name + "'");     // gcc
                programm.Note(v->e, "previous definition of '" + name + "'");  // gcc
            }
            if (!extern_flag)
                v->only_extern = false;
            scope_variables.push_back(v);
            return v;
        }
    }

    v = std::make_shared<CVariable>();
    v->e = node->e;
    v->type = node->ctype;
    v->name = name;
    v->only_extern = extern_flag;
    if (global) {
        if (!node->ctype.flag_static)
            programm.variables[name] = v;
        v->output_name = name;
        programm.AddVariable(v);
    } else if (node->ctype.flag_static) {
        v->output_name = "__s_" + /*current_function->name + "_" + */ name;  // TODO
        programm.AddVariable(v);
    } else {
        v->is_stack_variable = true;
        v->stack_offset = current_stack_size;

        current_stack_size += v->type.SizeOf(v->e);
        if (max_stack_size < current_stack_size)
            max_stack_size = current_stack_size;
    }
    scope_variables.push_back(v);
    return v;
}
