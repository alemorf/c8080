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

#include "ccalcconst.h"
#include "../tools/cthrow.h"
#include "../tools/nodeisnumber.h"

template <class T>
static T &GetNumber(CNodePtr &node) {
    if constexpr (std::is_same_v<T, int64_t>)
        return node->number.i;
    else if constexpr (std::is_same_v<T, uint64_t>)
        return node->number.u;
    else if constexpr (std::is_same_v<T, float>)
        return node->number.f;
    else if constexpr (std::is_same_v<T, double>)
        return node->number.d;
    else if constexpr (std::is_same_v<T, long double>)
        return node->number.ld;
    else
        return 0;
}

template <class T>
static bool CalcOperatorInternal(CNodePtr &node) {
    auto &r = GetNumber<T>(node);
    const auto a = GetNumber<T>(node->a);
    const auto b = GetNumber<T>(node->b);
    switch (node->operator_code) {
        case COP_COMMA:
            r = b;
            return true;
        case COP_ADD:
            r = a + b;
            return true;
        case COP_SUB:
            r = a - b;
            return true;
        case COP_MUL:
            r = a * b;
            return true;
        case COP_DIV:
            if (b == 0)
                CThrow(node, "division by zero");  // gcc
            r = a / b;
            return true;
        case COP_CMP_L:
            node->number.u = (a < b);
            return true;
        case COP_CMP_G:
            node->number.u = (a > b);
            return true;
        case COP_CMP_LE:
            node->number.u = (a <= b);
            return true;
        case COP_CMP_GE:
            node->number.u = (a >= b);
            return true;
        case COP_CMP_E:
            node->number.u = (a == b);
            return true;
        case COP_CMP_NE:
            node->number.u = (a != b);
            return true;
        case COP_LAND:
            node->number.u = (a && b);
            return true;
        case COP_LOR:
            node->number.u = (a || b);
            return true;
    }
    if constexpr (!std::is_floating_point_v<T>) {
        switch (node->operator_code) {
            case COP_MOD:
                if (b == 0)
                    CThrow(node, "division by zero");  // gcc
                r = a % b;
                return true;
            case COP_OR:
                r = a | b;
                return true;
            case COP_AND:
                r = a & b;
                return true;
            case COP_XOR:
                r = a ^ b;
                return true;
            case COP_SHR:
                r = a >> b;
                return true;
            case COP_SHL:
                r = a << b;
                return true;
        }
    }
    return false;
}

static bool CalcOperator(CNodePtr &node) {
    assert(node->a->ctype.GetAsmType() == node->b->ctype.GetAsmType());

    switch (node->a->ctype.GetAsmType()) {
        case CBT_INT8:
        case CBT_INT16:
        case CBT_INT32:
        case CBT_INT64:
            return CalcOperatorInternal<int64_t>(node);
        case CBT_UINT8:
        case CBT_UINT16:
        case CBT_UINT32:
        case CBT_UINT64:
            return CalcOperatorInternal<uint64_t>(node);
        case CBT_FLOAT:
            return CalcOperatorInternal<float>(node);
        case CBT_DOUBLE:
            return CalcOperatorInternal<double>(node);
        case CBT_LONG_DOUBLE:
            return CalcOperatorInternal<long double>(node);
    }
    return false;
}

template <class T>
static bool CalcMonoOperatorInternal(CNodePtr &node) {
    switch (node->mono_operator_code) {
        case MOP_PLUS:
            GetNumber<T>(node) = +GetNumber<T>(node->a);
            return true;
        case MOP_MINUS:
            GetNumber<T>(node) = -GetNumber<T>(node->a);
            return true;
        case MOP_NOT:
            node->number.u = !GetNumber<T>(node->a);
            return true;
    }
    if constexpr (!std::is_floating_point_v<T>) {
        switch (node->mono_operator_code) {
            case MOP_NEG:
                GetNumber<T>(node) = ~GetNumber<T>(node->a);
                return true;
        }
    }
    return false;
}

static bool CalcMonoOperator(CNodePtr &node) {
    switch (node->a->ctype.GetAsmType()) {
        case CBT_INT8:
        case CBT_INT16:
        case CBT_INT32:
        case CBT_INT64:
            return CalcMonoOperatorInternal<int64_t>(node);
        case CBT_UINT8:
        case CBT_UINT16:
        case CBT_UINT32:
        case CBT_UINT64:
            return CalcMonoOperatorInternal<uint64_t>(node);
        case CBT_FLOAT:
            return CalcMonoOperatorInternal<float>(node);
        case CBT_DOUBLE:
            return CalcMonoOperatorInternal<double>(node);
        case CBT_LONG_DOUBLE:
            return CalcMonoOperatorInternal<long double>(node);
    }
    return false;
}

template <class A, class T>
static bool CalcConvertInternal(A &to, CNodePtr &node) {
    switch (node->ctype.GetAsmType()) {
        case CBT_INT8:
        case CBT_INT16:
        case CBT_INT32:
        case CBT_INT64:  // TODO: Вывести предупреждение
            to = static_cast<T>(node->number.i);
            return true;
        case CBT_UINT8:
        case CBT_UINT16:
        case CBT_UINT32:
        case CBT_UINT64:  // TODO: Вывести предупреждение
            to = static_cast<T>(node->number.u);
            return true;
        case CBT_FLOAT:  // TODO: Вывести предупреждение
            to = static_cast<T>(node->number.f);
            return true;
        case CBT_DOUBLE:  // TODO: Вывести предупреждение
            to = static_cast<T>(node->number.d);
            return true;
        case CBT_LONG_DOUBLE:  // TODO: Вывести предупреждение
            to = static_cast<T>(node->number.ld);
            return true;
    }
    return false;
}

static bool CCalcConvert(CNodePtr &to_node, CNodePtr &node) {
    if (node->type != CNT_NUMBER)
        return false;

    switch (to_node->ctype.GetAsmType()) {
        case CBT_INT8:
            return CalcConvertInternal<int64_t, int8_t>(to_node->number.i, node);
        case CBT_UINT8:
            return CalcConvertInternal<uint64_t, uint8_t>(to_node->number.u, node);
        case CBT_INT16:
            return CalcConvertInternal<int64_t, int16_t>(to_node->number.i, node);
        case CBT_UINT16:
            return CalcConvertInternal<uint64_t, uint16_t>(to_node->number.u, node);
        case CBT_INT32:
            return CalcConvertInternal<int64_t, int32_t>(to_node->number.i, node);
        case CBT_UINT32:
            return CalcConvertInternal<uint64_t, uint32_t>(to_node->number.u, node);
        case CBT_INT64:
            return CalcConvertInternal<int64_t, int64_t>(to_node->number.i, node);
        case CBT_UINT64:
            return CalcConvertInternal<uint64_t, uint64_t>(to_node->number.u, node);
        case CBT_FLOAT:
            return CalcConvertInternal<float, float>(to_node->number.f, node);
        case CBT_DOUBLE:
            return CalcConvertInternal<double, double>(to_node->number.d, node);
        case CBT_LONG_DOUBLE:
            return CalcConvertInternal<long double, long double>(to_node->number.ld, node);
    }
    return false;
}

bool CCalcConst(CNodePtr &node, bool process_childs) {
    bool changed = false;
    for (;;) {
        switch (node->type) {
            case CNT_LOAD_VARIABLE:
                if (node->variable->type.IsConst() && node->variable->body) {
                    if (!node->variable->c_calc_const_executed) {
                        node->variable->c_calc_const_executed = true;       // Prevent recursion
                        changed |= CCalcConst(node->variable->body, true);  // TODO: Don't call everytime
                        node->variable->c_calc_const_executed = false;
                    }
                    if (node->variable->body->type == CNT_NUMBER) {
                        node->ctype = node->variable->body->ctype;
                        node->number = node->variable->body->number;
                        node->variable = nullptr;  // Free memory
                        node->type = CNT_NUMBER;
                        return true;
                    }
                }
                break;
            case CNT_CONVERT:
                if (process_childs)
                    changed |= CCalcConst(node->a, process_childs);
                if (CCalcConvert(node, node->a)) {
                    node->type = CNT_NUMBER;
                    node->a = nullptr;
                    return true;
                }
                break;
            case CNT_MONO_OPERATOR:
                if (process_childs)
                    changed |= CCalcConst(node->a, process_childs);
                if (node->a->type == CNT_NUMBER) {
                    if (CalcMonoOperator(node)) {
                        node->type = CNT_NUMBER;
                        node->a = nullptr;         // Free memory
                        CCalcConvert(node, node);  // Truncate 64 bit to 8, 16, 32
                        return true;
                    }
                }
                break;
            case CNT_OPERATOR: {
                if (node->operator_code == COP_IF) {
                    if (process_childs)
                        changed |= CCalcConst(node->a, process_childs);
                    if (node->a->type == CNT_NUMBER) {
                        DeleteNode(node, NodeIsNumber0(node->a) ? 'c' : 'b');
                        changed = true;
                        continue;  // RETRY!
                    }
                } else {
                    if (process_childs) {
                        changed |= CCalcConst(node->a, process_childs);
                        changed |= CCalcConst(node->b, process_childs);
                    }
                    if (node->a->type == CNT_NUMBER && node->b->type == CNT_NUMBER) {
                        if (CalcOperator(node)) {
                            node->type = CNT_NUMBER;
                            node->a = nullptr;         // Free memory
                            node->b = nullptr;         // Free memory
                            CCalcConvert(node, node);  // Truncate 64 bit to 8, 16, 32
                            return true;
                        }
                    }
                }
                break;
            }
        }
        break;
    }
    return changed;
}
