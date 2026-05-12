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

#include "cprepareargs.h"
#include <map>

void CPrepareArgs(CNodePtr node, CProgramm &programm) {
    assert(node && node->variable);

    CVariable &v = *node->variable;

    if (!v.type.IsFunction())
        return;

    if (!v.function_arguments.empty())
        return;  // Already created

    std::vector<CStructItem *> args;
    if (programm.GetVariableMode(v.type) == CVM_GLOBAL) {
        for (size_t i = node->ctype.function_args.size(); i > 1; i--)
            args.push_back(&node->ctype.function_args[i - 1]);
    } else {
        for (size_t i = 1; i < node->ctype.function_args.size(); i++)
            args.push_back(&node->ctype.function_args[i]);
    }

    uint64_t offset = 0;
    for (auto &item : args) {
        CVariablePtr a = std::make_shared<CVariable>();
        a->type = item->type;
        a->name = item->name;
        a->is_stack_variable = true;
        a->is_function_argument = true;
        a->e = node->e;

        auto argument_size = a->type.SizeOf(a->e);
        a->stack_offset = offset;
        if (argument_size == 1)
            offset++;  // The stack is word aligned
        offset += argument_size;

        if (programm.GetVariableMode(v.type) == CVM_GLOBAL)
            v.function_arguments.insert(v.function_arguments.begin(), a);
        else
            v.function_arguments.push_back(a);
    }
}
