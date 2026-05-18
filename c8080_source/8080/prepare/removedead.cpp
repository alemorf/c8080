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
#include "../../c/tools/nodeisnumber.h"

namespace I8080 {

bool PrepareRemoveDead(Prepare &, CNodePtr &node) {
    // 0 ? B : C  => C
    // 1 ? B : C  => B
    if (node->type == CNT_OPERATOR && node->operator_code == COP_IF && node->a->type == CNT_NUMBER) {
        DeleteNode(node, NodeIsNumber0(node->a) ? 'c' : 'b');
        return true;
    }

    // if (0) B else C  =>  { C }
    // if (1) B else C  =>  { B }
    if (node->type == CNT_IF && node->a->type == CNT_NUMBER && !node->has_label) {
        node->type = CNT_LEVEL;
        node->a = NodeIsNumber0(node->a) ? node->c : node->b;
        node->b = nullptr;
        node->c = nullptr;
        return true;
    }

    // while (0) B  =>  {}
    // while (1) B  =>  while () B
    if (node->type == CNT_WHILE && node->a != nullptr && node->a->type == CNT_NUMBER && !node->has_label) {
        if (NodeIsNumber0(node->a)) {
            node->type = CNT_LEVEL;
            node->a = nullptr;
            node->b = nullptr;
        } else {
            node->a = nullptr;  // No condition
        }
        return true;
    }

    // do { B } while (0)  =>  do { B } while ()
    // do { B } while (1)  =>  for (;;) { B );
    if (node->type == CNT_DO && node->a != nullptr && node->a->type == CNT_NUMBER) {
        if (NodeIsNumber0(node->a)) {
            node->a = nullptr;  // No condition
        } else {
            node->type = CNT_FOR;
            node->a = nullptr;  // No condition
            node->d = node->b;
            node->b = nullptr;
        }
        return true;
    }

    // for (A; 0; C) D  =>  { A }
    // for (A; 1; C) D  =>  for (A; ; C) D
    if (node->type == CNT_FOR && node->b != nullptr && node->b->type == CNT_NUMBER && !node->has_label) {
        if (NodeIsNumber0(node->b)) {
            node->type = CNT_LEVEL;
            node->b = nullptr;
            node->c = nullptr;
            node->d = nullptr;
        } else {
            node->b = nullptr;
        }
        return true;
    }
    return false;
}

}  // namespace I8080
