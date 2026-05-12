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

#include "Compiler.h"
#include "../../c/tools/numberiszero.h"

namespace I8080 {

void Compiler::BuildJumpIf(bool prepare, CNodePtr &node, bool jmp_if_true, AsmLabel *label) {
    switch (node->type) {
        case CNT_MONO_OPERATOR:
            if (node->mono_operator_code == MOP_NOT) {
                BuildJumpIf(prepare, node->a, !jmp_if_true, label);
                node->compiler.main.able = true;
                node->compiler.main.regs = node->a->compiler.main.regs;
                return;
            }
            break;
        case CNT_OPERATOR:
            if (node->operator_code == COP_CMP_E || node->operator_code == COP_CMP_NE) {
                if (NumberIsZero(node->b)) {
                    node->compiler.main.regs =
                        BuildJumpIfZero(prepare, node->a, node->operator_code == COP_CMP_E, jmp_if_true, label);
                    node->compiler.main.able = true;
                    return;
                }
                if (NumberIsZero(node->a)) {
                    node->compiler.main.regs =
                        BuildJumpIfZero(prepare, node->b, node->operator_code == COP_CMP_E, jmp_if_true, label);
                    node->compiler.main.able = true;
                    return;
                }
            }

            switch (node->operator_code) {
                case COP_CMP_E:
                case COP_CMP_NE:
                case COP_CMP_L_ADD_CONST:
                case COP_CMP_GE_ADD_CONST:
                case COP_CMP_L:
                case COP_CMP_GE: {
                    if (prepare) {
                        MeasureReset(node, REG_PREPARE);
                        BuildOperator(node);
                        return;
                    }
                    MeasureReset(node, R8_A);

                    switch (jmp_if_true ? node->operator_code : NegativeCompareOperator(node->operator_code)) {
                        case COP_CMP_E:
                            out.jz_label(label);
                            break;
                        case COP_CMP_NE:
                            out.jnz_label(label);
                            break;
                        case COP_CMP_L:
                            if (node->a->ctype.IsUnsigned())
                                out.jc_label(label);
                            else
                                out.jm_label(label);
                            break;
                        case COP_CMP_GE:
                            if (node->a->ctype.IsUnsigned())
                                out.jnc_label(label);
                            else
                                out.jp_label(label);
                            break;
                        case COP_CMP_L_ADD_CONST:
                            assert(node->a->ctype.IsUnsigned());
                            out.jnc_label(label);
                            break;
                        case COP_CMP_GE_ADD_CONST:
                            assert(node->a->ctype.IsUnsigned());
                            out.jc_label(label);
                            break;
                        default:
                            C_ERROR_UNSUPPORTED_OPERATOR(node);
                    }
                    return;
                }
                case COP_LAND:
                    if (jmp_if_true) {
                        AsmLabel *label2 = prepare ? nullptr : out.AllocLabel();
                        BuildJumpIf(prepare, node->a, false, label2);
                        BuildJumpIf(prepare, node->b, true, label);
                        if (!prepare)
                            out.label(label2);
                    } else {
                        BuildJumpIf(prepare, node->a, false, label);
                        BuildJumpIf(prepare, node->b, false, label);
                    }
                    node->compiler.main.able = true;
                    node->compiler.main.regs = node->a->compiler.main.regs | node->b->compiler.main.regs;
                    return;
                case COP_LOR:
                    if (jmp_if_true) {
                        BuildJumpIf(prepare, node->a, true, label);
                        BuildJumpIf(prepare, node->b, true, label);
                    } else {
                        AsmLabel *label2 = prepare ? nullptr : out.AllocLabel();
                        BuildJumpIf(prepare, node->a, true, label2);
                        BuildJumpIf(prepare, node->b, false, label);
                        if (!prepare)
                            out.label(label2);
                    }
                    node->compiler.main.able = true;
                    node->compiler.main.regs = node->a->compiler.main.regs | node->b->compiler.main.regs;
                    return;
            }
    }
    C_ERROR_UNSUPPORTED_NODE_TYPE(node);
}

}  // namespace I8080
