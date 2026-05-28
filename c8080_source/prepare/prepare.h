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

#include "../c/cprogramm.h"

class Prepare;

typedef bool (*PrepareFunctionType)(Prepare &p, CNodePtr &node);

namespace I8080 {
class Asm;
}

class Prepare {
public:
    CProgramm &programm;
    CVariablePtr function;
    const PrepareFunctionType *list;
    I8080::Asm &out;
    CVariablePtr memcpy;

    Prepare(CProgramm &p, I8080::Asm &o, CVariablePtr f, const PrepareFunctionType *l)
        : programm(p), function(f), list(l), out(o) {
    }
};

void PrepareFunction(CProgramm &cprogramm, CVariablePtr &p, const PrepareFunctionType *list, I8080::Asm &out);
uint32_t PrepareInt(Prepare &p, CNodePtr *pnode);
bool DeleteNodeSaveType(CNodePtr &node, char c);
