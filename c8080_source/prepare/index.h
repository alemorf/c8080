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
#include "../c/consts.h"
#include "prepare.h"

bool PrepareLocalVariablesInit(Prepare &p, CNodePtr &node);
bool PrepareReplaceDivMulWithShift(Prepare &p, CNodePtr &node);
bool PrepareRemoveUselessOperations(Prepare &p, CNodePtr &node);
bool PrepareStructItem(Prepare &p, CNodePtr &node);
bool PrepareArrayElement(Prepare &p, CNodePtr &node);
bool PrepareLoadVariable(Prepare &p, CNodePtr &node);
bool PrepareAddrDeaddr(Prepare &p, CNodePtr &node);
bool PrepareAddWithStackAddress(Prepare &p, CNodePtr &node);
bool PrepareDoubleConvert(Prepare &, CNodePtr &node);
bool PrepareCompareOperators(Prepare &, CNodePtr &node);
bool PrepareJump(Prepare &, CNodePtr &parent);
bool Prepare8BitVarArgs(Prepare &p, CNodePtr &node);
bool PrepareCopyStruct(Prepare &p, CNodePtr &node);
bool PrepareRemoveDead(Prepare &, CNodePtr &node);
