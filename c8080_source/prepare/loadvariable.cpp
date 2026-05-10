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

#include "index.h"
#include "../c/tools/makecnode.h"

// The C parser stores variables as CNT_LOAD_VARIABLE.
// The C compiler does not support CNT_LOAD_VARIABLE.
// Need to replace CNT_LOAD_VARIABLE with DEADDR(CNT_ARG_STACK_ADDRESS(N) / CNT_STACK_ADDRESS(N)).

bool PrepareLoadVariable(Prepare &p, CNodePtr &node) {
    if (node->type == CNT_LOAD_VARIABLE) {
        CVariablePtr &v = node->variable;
        if (v->is_stack_variable) {
            assert(p.function != nullptr);
            assert(p.programm.GetVariableMode(p.function->type) == CVM_STACK);
            node->type = (v->is_function_argument ? CNT_ARG_STACK_ADDRESS : CNT_STACK_ADDRESS);
            node->number.u = v->stack_offset;
            bool a = !v->type.IsArray();
            node->variable = nullptr;
            if (a) {
                node = MakeCNodeDeaddr(node);
                node->a->ctype.pointers.push_back(CPointer());
            }
            return true;
        }
    }
    return false;
}
