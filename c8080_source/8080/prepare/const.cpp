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
#include "../asm/asm.h"
#include "../../c/tools/ccalcconst.h"

namespace I8080 {

static void PrepareConstConvert(CNodePtr to_node, CNodePtr node, CType to_type, Asm &out) {
    std::vector<std::shared_ptr<CVariable>> use;  // to_name == node
    switch (to_type.GetAsmType()) {
        case CBT_INT8:
        case CBT_UINT8:
            to_node->text = "0FFh & (" + out.GetConst(node, nullptr, &use) + ")";
            break;
        case CBT_INT16:
        case CBT_UINT16:
            to_node->text = "0FFFFh & (" + out.GetConst(node, nullptr, &use) + ")";
            break;
        case CBT_INT32:
        case CBT_UINT32:
            to_node->text = "0FFFFFFFFh & (" + out.GetConst(node, nullptr, &use) + ")";
            break;
        case CBT_INT64:
        case CBT_UINT64:
            to_node->text = "0FFFFFFFFFFFFFFFFh & (" + out.GetConst(node, nullptr, &use) + ")";
            break;
        case CBT_FLOAT:
        case CBT_DOUBLE:
        case CBT_LONG_DOUBLE:
            to_node->text = out.GetConst(node, nullptr, &use);
            break;
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE_INT(to_node, to_type);
    }
    to_node->compiler.used_variables.swap(use);
    to_node->type = CNT_CONST;
}

static bool PrepareConstOperator(Prepare &p, CString op, CNodePtr node) {
    if (node->a->IsConstNode() && node->b->IsConstNode()) {
        node->text = "(" + p.out.GetConst(node->a, nullptr, &node->compiler.used_variables) + ") " + op + " (" +
                     p.out.GetConst(node->b, nullptr, &node->compiler.used_variables) + ")";
        node->type = CNT_CONST;
        node->a = nullptr;
        node->b = nullptr;
        PrepareConstConvert(node, node, node->ctype, p.out);
        return true;
    }
    return false;
}

static bool PrepareConstMono(Prepare &p, CString op, CNodePtr node) {
    if (node->a->IsConstNode()) {
        node->text = op + " (" + p.out.GetConst(node->a, nullptr, &node->compiler.used_variables) + ")";
        node->type = CNT_CONST;
        node->a = nullptr;
        PrepareConstConvert(node, node, node->ctype, p.out);
        return true;
    }
    return false;
}

bool PrepareConst(Prepare &p, CNodePtr &node) {
    switch (node->type) {
        case CNT_CONST_STRING:
            node->text = p.out.GetConst(node, nullptr, &node->compiler.used_variables);
            node->type = CNT_CONST;
            node->const_string = nullptr;
            return true;
        case CNT_CONVERT:
            if (node->a->IsConstNode() && !node->ctype.IsVoid()) {  // TODO: No void here!
                PrepareConstConvert(node, node->a, node->ctype, p.out);
                node->a = nullptr;
                return true;
            }
            return false;
        case CNT_MONO_OPERATOR:
#if 0  // TODO
            if (p.programm.cmm && node->mono_operator_code == MOP_ADDR && node->a->type == CNT_MONO_OPERATOR &&
                node->a->mono_operator_code == MOP_DEADDR) {
                DeleteNode(node, 'a');
                DeleteNode(node, 'a');
                return true;
            }
#endif
            if (node->a->IsConstNode()) {
                switch (node->mono_operator_code) {
                    case MOP_PLUS:
                        return PrepareConstMono(p, "+", node);
                    case MOP_MINUS:
                        return PrepareConstMono(p, "-", node);
                    case MOP_NOT:
                        return PrepareConstMono(p, "!", node);
                    case MOP_NEG:
                        return PrepareConstMono(p, "~", node);
                }
            }
            return false;
        case CNT_OPERATOR: {
            if (node->dont_replace_jump_node)
                return false;
            switch (node->operator_code) {
                case COP_ADD:
                    return PrepareConstOperator(p, "+", node);
                case COP_SUB:
                    return PrepareConstOperator(p, "-", node);
                case COP_MUL:
                    return PrepareConstOperator(p, "*", node);
                case COP_DIV:
                    return PrepareConstOperator(p, "/", node);
                case COP_MOD:
                    return PrepareConstOperator(p, "%", node);
                case COP_OR:
                    return PrepareConstOperator(p, "|", node);
                case COP_AND:
                    return PrepareConstOperator(p, "&", node);
                case COP_XOR:
                    return PrepareConstOperator(p, "^", node);
                case COP_CMP_L:
                    return PrepareConstOperator(p, "<", node);
                case COP_CMP_G:
                    return PrepareConstOperator(p, ">", node);
                case COP_CMP_LE:
                    return PrepareConstOperator(p, "<=", node);
                case COP_CMP_GE:
                    return PrepareConstOperator(p, ">=", node);
                case COP_CMP_E:
                    return PrepareConstOperator(p, "==", node);
                case COP_CMP_NE:
                    return PrepareConstOperator(p, "!=", node);
                case COP_SHR:
                    return PrepareConstOperator(p, ">>", node);
                case COP_SHL:
                    return PrepareConstOperator(p, "<<", node);
                case COP_LAND:
                    return PrepareConstOperator(p, "&&", node);
                case COP_LOR:
                    return PrepareConstOperator(p, "||", node);
                case COP_COMMA:
                    if (node->a->IsConstNode()) {
                        assert(node->b->ctype.GetAsmType() == node->ctype.GetAsmType());
                        DeleteNode(node, 'b');
                        return true;
                    }
                    return false;
                    // COP_IF is not supported by sjasm
            }
            return false;
        }
    }
    return false;
}

}  // namespace I8080
