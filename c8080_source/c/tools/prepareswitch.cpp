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

#include "prepareswitch.h"
#include <algorithm>

bool PrepareSwitch(CProgramm &p, CNodePtr &node, std::vector<CNodePtr> &cases) {
    int64_t min;
    uint64_t max;
    switch (node->a->ctype.GetAsmType()) {
        case CBT_INT8:
            min = INT8_MIN;
            max = INT8_MAX;
            break;
        case CBT_UINT8:
            min = 0;
            max = UINT8_MAX;
            break;
        case CBT_INT16:
            min = INT16_MIN;
            max = INT16_MAX;
            break;
        case CBT_UINT16:
            min = 0;
            max = UINT16_MAX;
            break;
        case CBT_INT32:
            min = INT32_MIN;
            max = INT32_MAX;
            break;
        case CBT_UINT32:
            min = 0;
            max = UINT32_MAX;
            break;
        case CBT_LONG_LONG:
            min = INT64_MIN;
            max = INT64_MAX;
            break;
        case CBT_UNSIGNED_LONG_LONG:
            min = 0;
            max = UINT64_MAX;
            break;
        default:
            p.Error(node->a->e, "switch quantity not an integer");  // gcc
            return false;
    }

    for (CNodePtr j = node->case_link.lock(); j; j = j->case_link.lock()) {
        if (j->a->type != CNT_NUMBER) {
            p.Error(j->a->e, "case label does not reduce to an integer constant");  // gcc
            continue;
        }
        switch (j->a->ctype.GetAsmType()) {
            case CBT_INT8:
            case CBT_INT16:
            case CBT_INT32:
            case CBT_INT64:
                if (j->a->number.i < min) {
                    p.Error(j->a->e, "case label value exceeds minimum value for type");  // gcc
                    continue;
                }
                if (j->a->number.i >= 0 && uint64_t(j->a->number.i) > max) {
                    p.Error(j->a->e, "case label value exceeds maximum value for type");  // gcc
                    continue;
                }
                j->number.u = uint64_t(j->a->number.i);
                break;
            case CBT_UINT8:
            case CBT_UINT16:
            case CBT_UINT32:
            case CBT_UINT64:
                if (j->a->number.u > max) {
                    p.Error(j->a->e, "case label value exceeds maximum value for type");  // gcc
                    continue;
                }
                j->number.u = j->a->number.u;
                break;
            default:
                p.Error(j->a->e, "case label does not reduce to an integer constant");  // gcc
                continue;
        }
        cases.push_back(j);
    }

    std::sort(cases.begin(), cases.end(), [&](CNodePtr &a, CNodePtr &b) { return a->number.u < b->number.u; });

    // TODO: Duplicate

    return true;
}
