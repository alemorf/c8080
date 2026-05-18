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

#include "makeoperator.h"
#include "cthrow.h"
#include "convert.h"
#include "nodeisnumber.h"

static void MakeOperatorError(CConstNodePtr n) {
    CThrow(n, std::string("no match for ") + ToString(n->operator_code) + " (operand types are " +
                  n->a->ctype.ToString() + " and " + n->b->ctype.ToString() + ")");  // gcc
}

static CType CalcResultCType(CConstNodePtr n, CConstType a, CConstType b) {
    if (!a.IsPointer() && !b.IsPointer()) {
        CBaseType r = CalcResultCBaseType(a.base_type, b.base_type);
        if (r == CBT_VOID)
            MakeOperatorError(n);
        return CType{r};
    }

    if (a.IsPointer() && b.IsPointer()) {
        if (a.CompareNoStatic(b))
            return a;

        // Convert array to pointer
        CType type_0_ = a;
        type_0_.pointers.back().ResetArray();
        CType type_1_ = b;
        type_1_.pointers.back().ResetArray();
        // TODO: Вычисление общего const

        if (type_0_.CompareNoStatic(type_1_))
            return type_0_;

        return CType{CBT_VOID, flag_const : a.IsConstPointer() || b.IsConstPointer(), pointers : {CPointer()}};
    }

    MakeOperatorError(n);
    return a;  // no return
}

static CType CalcResultCType(CConstNodePtr n, CConstNodePtr a, CConstNodePtr b) {
    if (a->ctype.IsPointer() && NodeIsNumber0(b))
        return a->ctype;

    if (NodeIsNumber0(a) && b->ctype.IsPointer())
        return b->ctype;

    return CalcResultCType(n, a->ctype, b->ctype);
}

static bool CanCastToBool(CConstType a) {
    if (a.IsPointer())
        return true;
    switch (a.base_type) {
        case CBT_CHAR:
        case CBT_UNSIGNED_CHAR:
        case CBT_SIGNED_CHAR:
        case CBT_SHORT:
        case CBT_UNSIGNED_SHORT:
        case CBT_INT:
        case CBT_UNSIGNED_INT:
        case CBT_LONG:
        case CBT_UNSIGNED_LONG:
        case CBT_LONG_LONG:
        case CBT_UNSIGNED_LONG_LONG:
        case CBT_FLOAT:
        case CBT_DOUBLE:
        case CBT_LONG_DOUBLE:
            return true;
    }
    return false;
}

static void MakeOperatorInt(CNodePtr &n, bool cmm) {
    switch (n->operator_code) {
        case COP_CMP_L:
        case COP_CMP_G:
        case COP_CMP_LE:
        case COP_CMP_GE:
        case COP_CMP_E:
        case COP_CMP_NE: {
            CType common_type = CalcResultCType(n, n->a, n->b);
            if (n->a->ctype != common_type)
                n->a = Convert(common_type, n->a, cmm);
            if (n->b->ctype != common_type)
                n->b = Convert(common_type, n->b, cmm);
            n->ctype = CType{CBT_BOOL};
            if (common_type.IsPointer())
                return;
            switch (common_type.base_type) {
                case CBT_CHAR:
                case CBT_UNSIGNED_CHAR:
                case CBT_SIGNED_CHAR:
                case CBT_SHORT:
                case CBT_UNSIGNED_SHORT:
                case CBT_INT:
                case CBT_UNSIGNED_INT:
                case CBT_LONG:
                case CBT_UNSIGNED_LONG:
                case CBT_LONG_LONG:
                case CBT_UNSIGNED_LONG_LONG:
                case CBT_FLOAT:
                case CBT_DOUBLE:
                case CBT_LONG_DOUBLE:
                    return;
                case CBT_STRUCT:
                case CBT_FUNCTION:
                case CBT_VA_LIST:
                    MakeOperatorError(n);
            }
            MakeOperatorError(n);
        }
        case COP_ADD:
        case COP_SUB:
        case COP_MUL:
        case COP_DIV:
            n->ctype = CalcResultCType(n, n->a, n->b);
            if (n->ctype.IsPointer())
                MakeOperatorError(n);
            n->a = Convert(n->ctype, n->a, cmm);
            n->b = Convert(n->ctype, n->b, cmm);
            switch (n->ctype.base_type) {
                case CBT_CHAR:
                case CBT_UNSIGNED_CHAR:
                case CBT_SIGNED_CHAR:
                case CBT_SHORT:
                case CBT_UNSIGNED_SHORT:
                case CBT_INT:
                case CBT_UNSIGNED_INT:
                case CBT_LONG:
                case CBT_UNSIGNED_LONG:
                case CBT_LONG_LONG:
                case CBT_UNSIGNED_LONG_LONG:
                case CBT_FLOAT:
                case CBT_DOUBLE:
                case CBT_LONG_DOUBLE:
                    return;
                case CBT_STRUCT:
                case CBT_FUNCTION:
                case CBT_VA_LIST:
                    MakeOperatorError(n);
            }
            MakeOperatorError(n);
        case COP_MOD:
        case COP_SHR:
        case COP_SHL:
        case COP_AND:
        case COP_OR:
        case COP_XOR:
            n->ctype = CalcResultCType(n, n->a, n->b);
            if (n->ctype.IsPointer())
                MakeOperatorError(n);
            n->a = Convert(n->ctype, n->a, cmm);
            n->b = Convert(n->ctype, n->b, cmm);
            switch (n->ctype.base_type) {
                case CBT_CHAR:
                case CBT_UNSIGNED_CHAR:
                case CBT_SIGNED_CHAR:
                case CBT_SHORT:
                case CBT_UNSIGNED_SHORT:
                case CBT_INT:
                case CBT_UNSIGNED_INT:
                case CBT_LONG:
                case CBT_UNSIGNED_LONG:
                case CBT_LONG_LONG:
                case CBT_UNSIGNED_LONG_LONG:
                    return;
                case CBT_FLOAT:
                case CBT_DOUBLE:
                case CBT_LONG_DOUBLE:
                case CBT_STRUCT:
                case CBT_FUNCTION:
                case CBT_VA_LIST:
                    MakeOperatorError(n);
            }
            MakeOperatorError(n);
        case COP_LAND:
        case COP_LOR:
            if (!CanCastToBool(n->a->ctype) || !CanCastToBool(n->b->ctype))
                MakeOperatorError(n);
            n->ctype = CType{CBT_BOOL};
            // Аргументы не приведены к bool, потому что это мешает работать
            // оптимизатору. Оптимизатор сам потом приведет аругменты к bool.
            return;
        case COP_SET:
            if (n->a->ctype.IsConst())
                CThrow(n->a, "assignment of read-only variable");  // gcc
            n->ctype = n->a->ctype;
            n->b = Convert(n->a->ctype, n->b, cmm);
            return;
        case COP_SET_ADD:
        case COP_SET_SUB:
        case COP_SET_MUL:
        case COP_SET_DIV:
            if (n->a->ctype.IsPointer())
                MakeOperatorError(n);
            if (n->a->ctype.IsConst())
                CThrow(n->a, "assignment of read-only variable");  // gcc
            n->ctype = n->a->ctype;
            n->b = Convert(n->a->ctype, n->b, cmm);
            switch (n->ctype.base_type) {
                case CBT_CHAR:
                case CBT_UNSIGNED_CHAR:
                case CBT_SIGNED_CHAR:
                case CBT_SHORT:
                case CBT_UNSIGNED_SHORT:
                case CBT_INT:
                case CBT_UNSIGNED_INT:
                case CBT_LONG:
                case CBT_UNSIGNED_LONG:
                case CBT_LONG_LONG:
                case CBT_UNSIGNED_LONG_LONG:
                case CBT_FLOAT:
                case CBT_DOUBLE:
                case CBT_LONG_DOUBLE:
                    return;
                case CBT_STRUCT:
                case CBT_FUNCTION:
                case CBT_VA_LIST:
                    MakeOperatorError(n);
            }
            MakeOperatorError(n);
        case COP_SET_MOD:
        case COP_SET_SHR:
        case COP_SET_SHL:
        case COP_SET_AND:
        case COP_SET_OR:
        case COP_SET_XOR:
            if (n->a->ctype.IsPointer())
                MakeOperatorError(n);
            if (n->a->ctype.IsConst())
                CThrow(n->a, "assignment of read-only variable");  // gcc
            n->ctype = n->a->ctype;
            n->b = Convert(n->a->ctype, n->b, cmm);
            switch (n->ctype.base_type) {
                case CBT_CHAR:
                case CBT_UNSIGNED_CHAR:
                case CBT_SIGNED_CHAR:
                case CBT_SHORT:
                case CBT_UNSIGNED_SHORT:
                case CBT_INT:
                case CBT_UNSIGNED_INT:
                case CBT_LONG:
                case CBT_UNSIGNED_LONG:
                case CBT_LONG_LONG:
                case CBT_UNSIGNED_LONG_LONG:
                    return;
                case CBT_FLOAT:
                case CBT_DOUBLE:
                case CBT_LONG_DOUBLE:
                case CBT_STRUCT:
                case CBT_FUNCTION:
                case CBT_VA_LIST:
                    MakeOperatorError(n);
            }
            MakeOperatorError(n);
        case COP_COMMA:
            n->ctype = n->b->ctype;
            return;
            // TODO:        case OP_IF:
    }
    MakeOperatorError(n);
}

CNodePtr MakeOperator(COperatorCode o, CNodePtr a, CNodePtr b, const CErrorPosition &e, bool cmm) {
    assert(a != nullptr);
    assert(b != nullptr);

    CNodePtr node = CNODE({CNT_OPERATOR, a : a, b : b, operator_code : o, e : e});

    // pointer += number, pointer -= number, pointer + number, pointer - number
    if ((o == COP_SET_ADD || o == COP_SET_SUB || o == COP_ADD || o == COP_SUB) && a->ctype.IsPointer() &&
        !b->ctype.IsPointer()) {
        if (IsSetOperator(o) && a->ctype.IsConst())
            CThrow(a, "assignment of read-only variable");  // gcc

        uint64_t element_size = a->ctype.SizeOfElement(a->e);

        node->ctype = a->ctype;
        node->b = Convert(CType{CBT_SIZE}, node->b, cmm);

        if (element_size > 1) {
            node->b = CNODE({
                CNT_OPERATOR,
                a : node->b,
                b : CNODE({
                    CNT_NUMBER,
                    ctype : CType{CBT_SIZE},
                    number : {u : element_size},
                    e  // TODO: lost
                    : node->e
                }),
                ctype : CType{CBT_SIZE},
                operator_code : COP_MUL,
                e : node->e
            });
        }
        return node;
    }

    if (o == COP_SUB && a->ctype.IsPointer() && b->ctype.IsPointer()) {
        CType common_type = CalcResultCType(node, node->a, node->b);

        uint64_t common_type_size = common_type.SizeOfElement(node->e);
        if (common_type_size == 0)
            common_type_size = 1;

        node->a->ctype = CType{CBT_SIZE};
        node->b->ctype = CType{CBT_SIZE};
        MakeOperatorInt(node, cmm);

        if (common_type_size > 1) {  // void* - void* eq uint8_t* - uint8_t*
            CNodePtr element_size =
                CNODE({CNT_NUMBER, ctype : CType{CBT_SIZE}, number : {u : common_type_size}, e : node->e});

            node = CNODE(
                {CNT_OPERATOR, a : node, b : element_size, ctype : node->ctype, operator_code : COP_DIV, e : node->e});
        }

        return node;
    }

    MakeOperatorInt(node, cmm);
    return node;
}

CNodePtr MakeOperatorIf(CNodePtr a, CNodePtr b, CNodePtr c, const CErrorPosition &e, bool cmm) {
    CNodePtr result = CNODE({CNT_OPERATOR, a, b, c, operator_code : COP_IF, e : e});
    CType common_type = CalcResultCType(result, result->b, result->c);
    result->b = Convert(common_type, result->b, cmm);
    result->c = Convert(common_type, result->c, cmm);
    result->ctype = common_type;
    return result;
}
