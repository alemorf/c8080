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
#include "../../prepare/prepare.h"

namespace I8080 {

static const PrepareFunctionType prepare_function_list[] = {
    PrepareConst,
    PrepareSetOperators,
    PrepareFast8BitMath,
    PrepareFast8BitOptimization,
    PrepareCompareOperators,
    PrepareIncDec,
    PrepareSub16ToAdd16,
    PrepareLoadVariable,
    nullptr,
};

void PrepareFunction(CProgramm &programm, CNodePtr &node, Asm &out) {
    PrepareFunction(programm, node->variable, prepare_function_list, out);
}

void PrepareVariable(CProgramm &programm, CVariablePtr &var, Asm &out) {
    if (!var->c.prepared && var->body && !var->type.IsFunction()) {
        var->c.prepared = true;  // Prevent recursion
        CVariablePtr no_function;
        Prepare p(programm, out, no_function, prepare_function_list);
        PrepareInt(p, &var->body);
    }
}

}  // namespace I8080
