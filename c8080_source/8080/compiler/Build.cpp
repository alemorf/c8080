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

namespace I8080 {

void Compiler::Build(CNodePtr &node, AsmRegister reg) {
    if (out.measure) {
        CBuildCase &c = node->compiler.Get(reg);
        if (!c.able) {
            C_ERROR_INTERNAL(node, "No compilation cases");
            return;
        }
        out.measure_metric += c.metric;
        out.measure_regs |= c.regs;
        return;
    }

    if (node->IsJumpNode()) {
        if (reg == REG_PREPARE)
            return BuildJumpIf(true, node, false, nullptr);

        const auto label_false = out.AllocLabel();
        BuildJumpIf(false, node, false, label_false);
        if (reg == REG_NONE) {
            out.label(label_false);
            return;
        }
        out.ld_a_n8(1);
        const auto label_exit = out.AllocLabel();
        out.jmp_label(label_exit);
        out.label(label_false);
        out.ld_a_n8(0);
        out.label(label_exit);
        return;
    }

    if (MeasureReset(node, reg))
        return;

    switch (node->type) {
        case CNT_CONST_STRING:
        case CNT_CONST:
        case CNT_NUMBER:
            return BuildConst(node);
        case CNT_CONVERT:
            return BuildConvert(node);
        case CNT_MONO_OPERATOR:
            return BuildMonoOperator(node);
        case CNT_OPERATOR:
            return BuildOperator(node);
        case CNT_LOAD_FROM_REGISTER: {
            AsmRegister real_reg = GetResultReg(node->ctype, false, false, node);
            Measure(node, real_reg, &Compiler::Case_Empty);
            return;
        }
        case CNT_STACK_ADDRESS:
            Measure(node, REG_NONE, &Compiler::Case_Empty);
            Measure(node, R16_HL, &Compiler::Case_StackAddress);
            return;
        case CNT_ARG_STACK_ADDRESS:
            Measure(node, REG_NONE, &Compiler::Case_Empty);
            Measure(node, R16_HL, &Compiler::Case_ArgStackAddress);
            return;
        case CNT_FUNCTION_CALL:
        case CNT_FUNCTION_CALL_ADDR:
            BuildCall(node);
            return;
        default:
            C_ERROR_UNSUPPORTED_NODE_TYPE(node);
    }
}

void Compiler::BuildOperator(CNodePtr &node) {
    if (node->operator_code == COP_IF) {
        BuildJumpIf(true, node->a, false, nullptr);
        Build(node->b);
        Build(node->c);

        Measure(node, REG_NONE, &Compiler::Case_If);

        AsmRegister main = GetResultReg(node->ctype, false, false, node);
        Measure(node, main, &Compiler::Case_If);

        if (node->b->compiler.alt.able && node->c->compiler.alt.able) {
            AsmRegister alt = GetResultReg(node->ctype, true, false, node);
            Measure(node, alt, &Compiler::Case_If);
        }
        return;
    }

    Build(node->a);
    Build(node->b);

    if (node->operator_code == COP_SET) {
        BuildSet(node);
        return;
    }

    Measure(node, REG_NONE, &Compiler::Case_Comma);

    if (node->operator_code == COP_COMMA) {
        AsmRegister main = GetResultReg(node->ctype, false, false, node);
        Measure(node, main, &Compiler::Case_Comma);
        return;
    }

    switch (node->a->ctype.GetAsmType()) {
        case CBT_INT8:
        case CBT_UINT8:
            BuildOperator8(node);
            return;
        case CBT_INT16:
        case CBT_UINT16:
            BuildOperator16(node);
            return;
        case CBT_INT32:
        case CBT_UINT32:
            BuildOperator32(node);
            return;
        default:
            C_ERROR_UNSUPPORTED_ASM_TYPE(node->a);
    }
}

void Compiler::BuildMonoOperator(CNodePtr &node) {
    assert(node->type == CNT_MONO_OPERATOR);

    Build(node->a);

    Measure(node, REG_NONE, &Compiler::Case_Direct);

    switch (node->mono_operator_code) {
        case MOP_ADDR: {
            p.Error(node->e, "lvalue required");  // gcc
            AsmRegister result_reg = GetResultReg(node->ctype, false, false, node);
            Measure(node, result_reg, &Compiler::Case_Empty);
            break;
        }
        case MOP_DEADDR:
            BuildLoad(node);
            break;
        case MOP_PLUS: {
            AsmRegister main_reg = GetResultReg(node->ctype, false, false, node);
            Measure(node, main_reg, &Compiler::Case_Direct);
            if (node->a->compiler.alt.able) {
                AsmRegister alt_reg = GetResultReg(node->ctype, true, false, node);
                Measure(node, alt_reg, &Compiler::Case_Direct);
            }
            break;
        }
        case MOP_MINUS:
            switch (node->ctype.GetAsmType()) {
                case CBT_INT8:
                case CBT_UINT8:
                    Measure(node, R8_A, &Compiler::Case_Minus8);
                    break;
                case CBT_INT16:
                case CBT_UINT16:
                    Measure(node, R16_HL, &Compiler::Case_Minus16);
                    break;
                case CBT_INT32:
                case CBT_UINT32:
                    Measure(node, R32_DEHL, &Compiler::Case_Minus32);
                    break;
                default:
                    C_ERROR_UNSUPPORTED_ASM_TYPE(node);
            }
            break;
        case MOP_NEG:
            switch (node->ctype.GetAsmType()) {
                case CBT_INT8:
                case CBT_UINT8:
                    Measure(node, R8_A, &Compiler::Case_Neg8);
                    break;
                case CBT_INT16:
                case CBT_UINT16:
                    Measure(node, R16_HL, &Compiler::Case_Neg16);
                    break;
                case CBT_INT32:
                case CBT_UINT32:
                    Measure(node, R32_DEHL, &Compiler::Case_Neg32);
                    break;
                default:
                    C_ERROR_UNSUPPORTED_ASM_TYPE(node);
            }
            break;
    }
}

bool Compiler::Case_StackAddress(CNodePtr &node, AsmRegister reg) {
    out.ld_reg_stack_addr(R16_HL, node->number.u);
    out.add_hl_sp();
    return true;
}

bool Compiler::Case_ArgStackAddress(CNodePtr &node, AsmRegister reg) {
    out.ld_reg_arg_stack_addr(R16_HL, node->number.u);
    out.add_hl_sp();
    return true;
}

bool Compiler::Case_Comma(CNodePtr &node, AsmRegister reg) {
    Build(node->a, REG_NONE);
    Build(node->b, reg);
    return true;
}

bool Compiler::Case_If(CNodePtr &node, AsmRegister reg) {
    const auto label1 = out.AllocLabel();
    const auto label2 = out.AllocLabel();
    BuildJumpIf(false, node->a, false, label1);
    Build(node->b, reg);
    out.jmp_label(label2);
    out.label(label1);
    Build(node->c, reg);
    out.label(label2);
    return true;
}

bool Compiler::Case_JumpNode(CNodePtr &node, AsmRegister reg) {
    const auto label_false = out.AllocLabel();
    BuildJumpIf(false, node, false, label_false);
    if (reg == REG_NONE) {
        out.label(label_false);
        node->compiler.no_result.able = false;
        return true;
    }
    out.ld_a_n8(1);
    const auto label_exit = out.AllocLabel();
    out.jmp_label(label_exit);
    out.label(label_false);
    out.ld_a_n8(0);
    out.label(label_exit);
    node->compiler.main.able = false;
    return true;
}

bool Compiler::Case_Empty(CNodePtr &node, AsmRegister reg) {
    return true;
}

bool Compiler::Case_Direct(CNodePtr &node, AsmRegister reg) {
    assert(reg == REG_NONE);
    Build(node->a, REG_NONE);
    return true;
}

bool Compiler::Case_Minus8(CNodePtr &node, AsmRegister reg) {
    assert(reg == R8_A);
    Build(node->a, R8_A);
    out.cpl();
    out.inc_a();
    return true;
}

bool Compiler::Case_Minus16(CNodePtr &node, AsmRegister reg) {
    assert(reg == R16_HL);
    Build(node->a, R16_HL);
    InternalCall(o.minus_16);
    return true;
}

bool Compiler::Case_Minus32(CNodePtr &node, AsmRegister reg) {
    assert(reg == R32_DEHL);
    Build(node->a, R32_DEHL);
    InternalCall(o.minus_32);
    return true;
}

bool Compiler::Case_Neg8(CNodePtr &node, AsmRegister reg) {
    assert(reg == R8_A);
    Build(node->a, R8_A);
    out.cpl();
    return true;
}

bool Compiler::Case_Neg16(CNodePtr &node, AsmRegister reg) {
    assert(reg == R16_HL);
    Build(node->a, R16_HL);
    InternalCall(o.neg_16);
    return true;
}

bool Compiler::Case_Neg32(CNodePtr &node, AsmRegister reg) {
    assert(reg == R32_DEHL);
    Build(node->a, R32_DEHL);
    InternalCall(o.neg_32);
    return true;
}

}  // namespace I8080
