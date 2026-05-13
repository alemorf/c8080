/*
 * c8080 compiler
 * Copyright (c) 2026 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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

// Convert 8-bit variadic and stack function arguments to 16-bit.
// from:
//   char a;
//   unsigned char b;
//   printf("%i %u", a, b);
// to:
//   unsigned char b;
//   printf("%i %u", (int)a, (usigned)b);

static CNodePtr ConvertToIntUint(CNodePtr &arg) {
    assert(arg->next_node == nullptr);
    return CNODE({CNT_CONVERT, a : arg, ctype : CType{arg->ctype.IsSigned() ? CBT_INT : CBT_UNSIGNED_INT}, e : arg->e});
}

bool Prepare8BitVarArgs(Prepare &p, CNodePtr &node) {
    if (!p.programm.cmm && node->type == CNT_FUNCTION_CALL) {
        // 0 element of function_args is the return type
        const size_t static_arg_count = p.programm.GetVariableMode(node->variable->type) == CVM_GLOBAL
                                            ? (node->variable->type.function_args.size() - 1)
                                            : 0;

        size_t arg_number = 0;
        bool changed = false;
        for (CNodePtr *arg = &node->a; *arg; arg = &((*arg)->next_node)) {
            if (arg_number >= static_arg_count && (*arg)->ctype.Is8BitType()) {
                CNodePtr next_node = (*arg)->next_node;
                (*arg)->next_node = nullptr;
                *arg = ConvertToIntUint(*arg);
                (*arg)->next_node = next_node;
                changed = true;
            }
            arg_number++;
        }
        return changed;
    }
    return false;
}
