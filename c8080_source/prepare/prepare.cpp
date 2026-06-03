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

#include "prepare.h"
#include "index.h"
#include "../c/tools/ccalcconst.h"
#include "../c/tools/cthrow.h"
#include "staticstack.h"

bool DeleteNodeSaveType(CNodePtr &node, char c) {
    CType type = node->ctype;
    DeleteNode(node, c);
    if (node->ctype.GetAsmTypeIgnoreSign() != type.GetAsmTypeIgnoreSign())
        CThrow(node, "Internal error in " + std::string(__PRETTY_FUNCTION__));
    node->ctype = type;
    return true;
}

typedef bool (*PrepareFunctionType)(Prepare &p, CNodePtr &node);

static const PrepareFunctionType prepare_function_list[] = {
    Prepare8BitVarArgs,
    PrepareRemoveUselessOperations,
    PrepareReplaceDivMulWithShift,
    PrepareStructItem,
    PrepareArrayElement,
    PrepareLocalVariablesInit,
    PrepareAddrDeaddr,
    PrepareStaticLoadVariable,
    PrepareLoadVariable,
    PrepareStaticArgumentsCall,
    PrepareAddWithStackAddress,
    PrepareDoubleConvert,
    PrepareCompareOperators,
    PrepareCopyStruct,
    PrepareRemoveDead,
    nullptr,
};

enum { PREPARE_CHANGED = 1, PREPARE_LABEL = 2 };

uint32_t PrepareInt(Prepare &p, CNodePtr *pnode) {
    uint32_t return_value = 0;
    assert(pnode);
    while (*pnode != nullptr) {
        uint32_t r;
        do {
            r = 0;

            if (!p.programm.cmm)
                r = PrepareJump(p, *pnode);

            r |= PrepareInt(p, &(*pnode)->a);
            r |= PrepareInt(p, &(*pnode)->b);
            r |= PrepareInt(p, &(*pnode)->c);
            r |= PrepareInt(p, &(*pnode)->d);

            if (CCalcConst(*pnode, false))
                r |= PREPARE_CHANGED;

            if ((*pnode)->type == CNT_LABEL)
                r |= PREPARE_LABEL;
            if (r & PREPARE_LABEL)
                (*pnode)->has_label = true;

            for (auto i = prepare_function_list; *i && *pnode != nullptr; i++)
                if ((*i)(p, *pnode))
                    r |= PREPARE_CHANGED;

            for (auto i = p.list; *i && *pnode != nullptr; i++)
                if ((*i)(p, *pnode))
                    r |= PREPARE_CHANGED;

            return_value |= r;
        } while ((r & PREPARE_CHANGED) != 0 && *pnode != nullptr);

        if (*pnode != nullptr)
            pnode = &(*pnode)->next_node;
    }
    return return_value;
}

void PrepareFunction(CProgramm &programm, CVariablePtr &f, const PrepareFunctionType *list, I8080::Asm &out) {
    Prepare p(programm, out, f, list);
    PrepareFunctionStaticStack(p);
    PrepareInt(p, &p.function->body->a);
}
