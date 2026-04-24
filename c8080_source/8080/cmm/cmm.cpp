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

#include "cmm.h"
#include "names.h"
#include <limits.h>
#include "../asm/asm.h"
#include "../CompileVariable.h"
#include "../cmm/prepare.h"
#include "../../tools/strtouint64.h"
#include "../../c/tools/cthrow.h"
#include "../../c/tools/getnumberasuint64.h"
#include "arg.h"

namespace I8080 {

class Cmm {
public:
    CProgramm &p;
    Asm out;
    AsmLabel *break_label{};
    AsmLabel *continue_label{};

    typedef CmmArg Arg;

    Cmm(CProgramm &p_) : p(p_), out(p) {
    }
    void Compile(CString asm_file_name);
    void CompileDeclareVariable(CNodePtr &n);
    AsmCondition CompileCond(CNode &n);
    void CompileArg(CNodePtr &n, Arg &arg);
    void CompileLine(CNodePtr &n, Arg &out_arg);
    void CompileOperatorError(CNodePtr &n, Arg &a, Arg &b);
    void CompileAlu(CNodePtr &n, AsmAlu alu, Arg &a, Arg &b);
    void CompileLevel(CNodePtr &n);
    void CompileInternalFunctionCall(CNodePtr &n, Arg &result_value);
    void CompileRotate(CNodePtr &n, AssemblerCommand opcode);
    void CompileAlu(CNodePtr &n, AsmAlu alu);
    bool IsDoNothingArgs(CNodePtr &n);
    AsmCondition CmmNameToAsmCondition(CNode &v);
    void CompileArgs_Empty(CNodePtr &n);
    void CompileArgs_A(CNodePtr &n);
    uint64_t CompileArgs_A_OptionalNumber(CNodePtr &n, uint64_t value, uint64_t max_value);
    void CompilePushPop(CNodePtr &n, bool push, bool pop);
};

void Cmm::CompileArg(CNodePtr &n, Arg &arg) {
    bool addr = n->IsDeaddr();
    CompileLine(addr ? n->a : n, arg);
    if (arg.addr)
        p.Error(n->e, "can't compile");
    arg.addr = addr;
    if (arg.addr && arg.reg == R16_HL) {
        arg.addr = false;
        arg.reg = R8_M;
    }
}

AsmCondition Cmm::CmmNameToAsmCondition(CNode &n) {
    if (n.variable->c.internal_cmm_name == CMM_NAME_FLAG)
        return n.variable->c.asm_condition;
    p.Error(n.e, "only flag_");
    return JC_Z;
}

AsmCondition Cmm::CompileCond(CNode &n) {
    switch (n.type) {
        case CNT_OPERATOR: {
            assert(n.a && n.b);

            Arg a, b;
            CompileArg(n.a, a);
            CompileArg(n.b, b);

            if (!a.IsA())
                p.Error(n.a->e, "only register a as left argument of operator");

            if ((n.operator_code == COP_CMP_E || n.operator_code == COP_CMP_NE) && b.Is0())
                out.alu_a_reg(ALU_OR, R8_A);
            else if (b.Is8M())
                out.alu_a_reg(ALU_CMP, b.reg);
            else if (b.IsConst())
                out.alu_a_string(ALU_CMP, b.text);
            else
                p.Error(n.b->e, "only 8 bit register, *hl or const as right argument of operator");

            switch (n.operator_code) {
                case COP_CMP_L:
                    return JC_C;
                case COP_CMP_GE:
                    return JC_NC;
                case COP_CMP_E:
                    return JC_Z;
                case COP_CMP_NE:
                    return JC_NZ;
            }

            p.Error(n.e, "only < >= == !=");
            return JC_Z;
        }
        case CNT_MONO_OPERATOR:  // Example: if (flag_z) ...
            if (n.mono_operator_code == MOP_ADDR && n.a->type == CNT_LOAD_VARIABLE)
                return CmmNameToAsmCondition(*n.a);
            p.Error(n.e, std::string("unsupported mono operator ") + ToString(n.mono_operator_code));
            return JC_Z;
        case CNT_FUNCTION_CALL:  // Example: if (flag_z(...)) ...
            CompileLevel(n.a);
            return CmmNameToAsmCondition(n);
    }
    p.Error(n.e, "can't compile node " + ToString(n.type));
    return JC_Z;
}

void Cmm::CompileArgs_Empty(CNodePtr &n) {
    if (n->a)
        p.Error(n->e, "only ()");
}

void Cmm::CompileArgs_A(CNodePtr &n) {
    if (n->a && !n->a->next_node) {
        Arg a;
        CompileArg(n->a, a);
        if (a.IsA())
            return;
    }
    p.Error(n->e, "only (a)");
}

uint64_t Cmm::CompileArgs_A_OptionalNumber(CNodePtr &n, uint64_t default_value, uint64_t max_value) {
    if (n->a && (!n->a->next_node || !n->a->next_node->next_node)) {
        Arg a, b;
        CompileArg(n->a, a);
        if (a.IsA()) {
            if (!n->a->next_node)
                return default_value;
            CompileArg(n->a->next_node, b);
            uint64_t value = 0;
            if (b.IsConstNumber(value)) {
                if (value > max_value) {
                    p.Error(n->b->e, "maximal value is " + std::to_string(max_value));
                    value = max_value;
                }
                return value;
            }
        }
    }
    p.Error(n->e, "only (a) or (a, const number)");
    return default_value;
}

void Cmm::CompileRotate(CNodePtr &n, AssemblerCommand opcode) {
    uint64_t count = CompileArgs_A_OptionalNumber(n, 1, 8);
    for (uint64_t i = 0; i < count; i++)
        out.rotate(opcode);
}

void Cmm::CompilePushPop(CNodePtr &n, bool push, bool pop) {
    if (pop && !IsDoNothingArgs(n->a))
        p.Error(n->e, "only register names in arguments");

    std::vector<AsmRegister> items;

    for (auto i = n->a; i; i = i->next_node) {
        Arg a;
        CompileArg(i, a);
        if (a.IsA()) {
            a.reg = R16_AF;
        } else if (!a.Is16Af()) {
            p.Error(n->e, "only a, bc, de, hl");
            a.reg = R16_HL;
        }
        if (pop)
            items.push_back(a.reg);
        if (push)
            out.push_reg(a.reg);
    }

    CompileLevel(n->b);

    for (auto i = items.rbegin(); i != items.rend(); i++)
        out.pop_reg(*i);
}

void Cmm::CompileInternalFunctionCall(CNodePtr &n, Arg &result_value) {
    switch (n->variable->c.internal_cmm_name) {
        case CMM_NAME_SET_FLAG_C:
            CompileArgs_Empty(n);
            return out.stc();
        case CMM_NAME_INVERT:
            CompileArgs_A(n);
            return out.cpl();
        case CMM_NAME_ENABLE_INTERRUPTS:
            CompileArgs_Empty(n);
            return out.ei();
        case CMM_NAME_DISABLE_INTERRUPTS:
            CompileArgs_Empty(n);
            return out.di();
        case CMM_NAME_NOP:
            CompileArgs_Empty(n);
            return out.nop();
        case CMM_NAME_DAA:
            CompileArgs_Empty(n);
            return out.daa();
        case CMM_NAME_CYCLIC_ROTATE_LEFT:
            return CompileRotate(n, AC_RLCA);
        case CMM_NAME_CYCLIC_ROTATE_RIGHT:
            return CompileRotate(n, AC_RRCA);
        case CMM_NAME_CARRY_ROTATE_LEFT:
            return CompileRotate(n, AC_RLA);
        case CMM_NAME_CARRY_ROTATE_RIGHT:
            return CompileRotate(n, AC_RRA);
        case CMM_NAME_COMPARE:
            return CompileAlu(n, ALU_CMP);
        case CMM_NAME_CARRY_ADD:
            return CompileAlu(n, ALU_ADC);
        case CMM_NAME_CARRY_SUB:
            return CompileAlu(n, ALU_SBC);
        case CMM_NAME_PUSH:
            return CompilePushPop(n, true, false);
        case CMM_NAME_POP:
            return CompilePushPop(n, false, true);
        case CMM_NAME_IN:
            if (n->a && !n->a->next_node)
                return result_value.SetPort(out.GetConst(n->a));
            result_value.SetPort(0);
            return p.Error(n->e, "only (const)");
        case CMM_NAME_OUT:
            if (n->a && n->a->next_node && !n->a->next_node->next_node) {
                Arg a, b;
                CompileArg(n->a, a);
                CompileArg(n->a->next_node, b);
                if (a.IsConst() && b.IsA())
                    return out.out(a.text);
            }
            return p.Error(n->e, "only (const, a)");
        case CMM_NAME_SWAP:
            if (n->a && n->a->next_node && !n->a->next_node->next_node) {
                Arg a, b;
                CompileArg(n->a, a);
                CompileArg(n->a->next_node, b);
                if ((a.IsReg(R16_DE) && b.IsHl()) || (b.IsReg(R16_DE) && a.IsHl()))
                    return out.ex_hl_de();
                if ((a.IsRegAddr(R16_SP) && b.IsHl()) || (b.IsRegAddr(R16_SP) && a.IsHl()))
                    return out.ex_psp_hl();
            }
            return p.Error(n->e, "only (hl, de) or (*sp, hl)");
        default:
            p.Error(n->e, "can't compile " + n->variable->name + "()");
    }
}

bool Cmm::IsDoNothingArgs(CNodePtr &n) {
    for (CNodePtr i = n; i; i = i->next_node)
        if (i->type != CNT_CONST && i->type != CNT_NUMBER && i->type != CNT_LOAD_VARIABLE)
            return false;
    return true;
}

void Cmm::CompileOperatorError(CNodePtr &n, Arg &a, Arg &b) {
    p.Error(n->e, std::string("can't compile ") + ToString(a) + " " + ToString(n->operator_code) + " " + ToString(b));
}

void Cmm::CompileAlu(CNodePtr &n, AsmAlu alu) {
    if (n->a && n->a->next_node && !n->a->next_node->next_node) {
        Arg a, b;
        CompileArg(n->a, a);
        CompileArg(n->a->next_node, b);
        if (a.IsA()) {
            if (b.Is8M())
                return out.alu_a_reg(alu, b.reg);
            if (b.IsConst())
                return out.alu_a_string(alu, b.text);
        }
    }
    p.Error(n->e, "only (a, 8 bit register) or (a, *hl) or (a, const)");
}

void Cmm::CompileAlu(CNodePtr &n, AsmAlu alu, Arg &a, Arg &b) {
    if (a.IsHl() && b.Is16Sp())
        return out.add_hl_reg(b.reg);
    if (a.IsA()) {
        if (b.Is8M())
            return out.alu_a_reg(alu, b.reg);
        if (b.IsConst())
            return out.alu_a_string(alu, b.text);
    }
    CompileOperatorError(n, a, b);
}

void Cmm::CompileLine(CNodePtr &n, Arg &out_arg) {
    bool error = false;
    Arg b;
    out.source(n->e);
    switch (n->type) {
        case CNT_LOAD_VARIABLE:
            if (n->variable->c.internal_cmm_name == CMM_NAME_REG) {
                out_arg.reg = n->variable->c.asm_register;
                return;
            }
            p.Error(n->e, "can't get address of internal function");
            out_arg.SetReg(R8_A);
            return;
        case CNT_LEVEL:
            return CompileLevel(n->a);
        case CNT_CONST:
            return out_arg.SetConst(out.GetConst(n));
        case CNT_NUMBER:
            return out_arg.SetConst(out.GetConst(n));
        case CNT_PUSH_POP:
            return CompilePushPop(n, true, true);
        case CNT_FUNCTION_CALL:
            if (n->variable->c.internal_cmm_name)
                return CompileInternalFunctionCall(n, out_arg);
            CompileLevel(n->a);
            return out.call(n->variable->output_name);
        case CNT_FUNCTION_CALL_ADDR:
            CompileLevel(n->a);
            CompileArg(n->b, b);
            if (!b.IsConst())
                p.Error(n->b->e, "only const");
            return out.call(b.text);
        case CNT_RETURN:
            if (n->a) {
                if (n->a->type == CNT_FUNCTION_CALL_ADDR && n->a->b->type == CNT_LOAD_VARIABLE &&
                    n->a->b->variable->c.internal_cmm_name == CMM_NAME_REG &&
                    n->a->b->variable->c.asm_register == R16_HL) {
                    if (n->a->b->a)
                        p.Error(n->e, "only return hl()");
                    return out.pchl();
                }
                if (n->a->type == CNT_FUNCTION_CALL && n->a->variable->c.internal_cmm_name == CMM_NAME_REG &&
                    n->a->variable->c.asm_register == R16_HL) {
                    if (n->a->a)
                        p.Error(n->e, "only return hl()");
                    return out.pchl();
                }
                Arg arg2;
                CompileLine(n->a, arg2);
            }
            if (!out.lines.empty()) {
                auto &b = out.lines.back();
                if (b.opcode == AC_CALL) {
                    b.opcode = AC_JMP;
                    return;
                }
                if (b.opcode == AC_CALL_CONDITION) {
                    b.opcode = AC_JMP_CONDITION;
                    return;
                }
            }
            return out.ret();
        case CNT_BREAK:
            if (!break_label)
                return p.Error(n->e, "break without loop");  // TODO: Move to parser
            return out.jmp_label(break_label);
        case CNT_CONTINUE:
            if (!continue_label)
                return p.Error(n->e, "continue without loop");  // TODO: Move to parser
            return out.jmp_label(continue_label);
        case CNT_FOR: {
            CompileLevel(n->a);
            const auto continue_label_ = continue_label;
            const auto break_label_ = break_label;
            const auto for_label = out.AllocLabel();
            continue_label = out.AllocLabel();
            break_label = out.AllocLabel();
            out.label(n->c ? for_label : continue_label);
            if (n->b)
                out.jmp_condition_label(InvertAsmCondition(CompileCond(*n->b)), break_label);
            CompileLevel(n->d);
            if (n->c) {
                out.label(continue_label);
                CompileLevel(n->c);
            }
            out.jmp_label(n->c ? for_label : continue_label);
            if (break_label->used != 0)
                out.label(break_label);
            break_label = break_label_;
            continue_label = continue_label_;
            break;
        }
        case CNT_DO: {
            const auto loop_label = out.AllocLabel();
            const auto prev_continue_label = continue_label;
            const auto prev_break_label = break_label;
            continue_label = out.AllocLabel();
            break_label = out.AllocLabel();
            out.label(n->a ? loop_label : continue_label);
            CompileLevel(n->b);
            if (n->a) {
                out.label(continue_label);
                AsmCondition cond = CompileCond(*n->a);
                out.jmp_condition_label(cond, loop_label);
            } else {
                out.jmp_label(n->a ? loop_label : continue_label);
            }
            if (break_label->used != 0)
                out.label(break_label);
            break_label = prev_break_label;
            continue_label = prev_continue_label;
            break;
        }
        case CNT_IF: {
            AsmCondition cond = CompileCond(*n->a);

            // One "true" command and no "else" commands
            if (n->b && !n->b->next_node && !n->c) {
                out.source(n->b->e);

                if (n->b->type == CNT_RETURN) {
                    if (!n->b->a)
                        return out.ret_condition(cond);
                    if (n->b->a->type == CNT_FUNCTION_CALL && n->b->a->variable->c.internal_cmm_name == 0 &&
                        IsDoNothingArgs(n->b->a->a) && !n->b->a->next_node) {
                        return out.jmp_condition(cond, n->b->a->variable->output_name);
                    }
                }
                if (n->b->type == CNT_FUNCTION_CALL && n->b->variable->c.internal_cmm_name == 0 &&
                    IsDoNothingArgs(n->b->a)) {
                    return out.call_condition(cond, n->b->variable->output_name);
                }
                if (n->b->type == CNT_GOTO)
                    return out.jmp_condition(cond, n->b->variable->output_name);
                if (n->b->type == CNT_BREAK) {
                    if (!break_label)
                        return p.Error(n->e, "break without loop");
                    return out.jmp_condition_label(cond, break_label);
                }
                if (n->b->type == CNT_CONTINUE) {
                    if (!continue_label)
                        return p.Error(n->e, "continue without loop");
                    return out.jmp_condition_label(cond, continue_label);
                }
            }

            const auto else_label = out.AllocLabel();
            const auto endif_label = out.AllocLabel();
            out.jmp_condition_label(InvertAsmCondition(cond), else_label);
            CompileLevel(n->b);
            if (n->c)
                out.jmp_label(endif_label);
            out.label(else_label);
            if (n->c) {
                CompileLevel(n->c);
                out.label(endif_label);
            }
            break;
        }
        case CNT_OPERATOR:
            CompileArg(n->a, out_arg);
            if (n->operator_code == COP_SET_SHL && n->b->type == CNT_NUMBER) {
                const uint64_t count = GetNumberAsUint64(n->b) % 64;
                for (unsigned i = 0; i < count; i++)
                    CompileAlu(n, ALU_ADD, out_arg, out_arg);
                return;
            }
            CompileArg(n->b, b);
            switch (n->operator_code) {
                case COP_SET:
                    if (b.IsPort() && out_arg.IsA())
                        return out.in(b.text);
                    if (out_arg.Is8M() && b.Is8M() && !(out_arg.reg == R8_M && b.reg == R8_M))
                        return out.ld_r8_r8(out_arg.reg, b.reg);
                    if (out_arg.Is8M() && b.IsConst())
                        return out.ld_r8_string(out_arg.reg, b.text);
                    if (out_arg.Is16Sp() && b.IsConst())
                        return out.ld_r16_string(out_arg.reg, b.text);
                    if (out_arg.IsConstAddr() && b.IsA())
                        return out.ld_pstring1_a(out_arg.text);
                    if (out_arg.IsConstAddr() && b.IsHl())
                        return out.ld_pstring_hl(out_arg.text);
                    if (out_arg.IsA() && b.IsConstAddr())
                        return out.ld_a_pstring(b.text);
                    if (out_arg.IsHl() && b.IsConstAddr())
                        return out.ld_hl_pstring(b.text);
                    if (out_arg.IsReg(R16_SP) && b.IsHl())
                        return out.ld_sp_hl();
                    if (out_arg.IsA() && (b.IsRegAddr(R16_BC) || b.IsRegAddr(R16_DE)))
                        return out.ld_a_preg(b.reg);
                    if ((out_arg.IsRegAddr(R16_BC) || out_arg.IsRegAddr(R16_DE)) && b.IsA())
                        return out.ld_preg_a(out_arg.reg);
                    return CompileOperatorError(n, out_arg, b);
                case COP_SET_ADD:
                    return CompileAlu(n, ALU_ADD, out_arg, b);
                case COP_SET_AND:
                    return CompileAlu(n, ALU_AND, out_arg, b);
                case COP_SET_OR:
                    return CompileAlu(n, ALU_OR, out_arg, b);
                case COP_SET_XOR:
                    return CompileAlu(n, ALU_XOR, out_arg, b);
                case COP_SET_SUB:
                    return CompileAlu(n, ALU_SUB, out_arg, b);
            }
            out_arg.SetConst(out.GetConst(n, &error));  // Build an expression for assembler
            if (error)
                CompileOperatorError(n, out_arg, b);
            return;
        case CNT_MONO_OPERATOR:
            switch (n->mono_operator_code) {
                case MOP_POST_INC:
                    CompileArg(n->a, out_arg);
                    if (!out_arg.Is16Sp() && !out_arg.Is8M()) {
                        p.Error(n->a->e, "only register or *hl");
                        out_arg.SetReg(R8_A);
                    }
                    return out.inc_reg(out_arg.reg);
                case MOP_POST_DEC:
                    CompileArg(n->a, out_arg);
                    if (!out_arg.Is16Sp() && !out_arg.Is8M()) {
                        p.Error(n->a->e, "only register or *hl");
                        out_arg.SetReg(R8_A);
                    }
                    return out.dec_reg(out_arg.reg);
                case MOP_ADDR:
                    out_arg.SetConst(out.GetConst(n->a));
                    return;
                default:
                    CompileArg(n->a, out_arg);
                    p.Error(n->e, std::string("can't compile ") + ToString(n->mono_operator_code) + ToString(out_arg));
                    out_arg.SetReg(R8_A);
            }
            break;
        case CNT_LABEL:
            out.label(n->variable->output_name);
            return;
        case CNT_GOTO:
            return out.jmp(n->variable->output_name);
        case CNT_ASM:
            return out.assembler(n->text.c_str());
        case CNT_DECLARE_VARIABLE:
            out.MakeFile();
            return CompileVariable(out, p, n->variable);
        default:
            p.Error(n->e, "unsupported node " + ToString(n->type));
    }
}

void Cmm::CompileLevel(CNodePtr &n) {
    for (CNodePtr i = n; i; i = i->next_node) {
        Arg arg;
        CompileLine(i, arg);
    }
}

void Cmm::CompileDeclareVariable(CNodePtr &n) {
    CVariable &v = *n->variable;

    if (n->extern_flag || (v.type.pointers.empty() && v.type.flag_const))
        return;

    // The last command of the function calls the next function
    if (!out.lines.empty()) {
        auto &b = out.lines.back();
        if (b.opcode == AC_JMP && b.argument[0].string == v.output_name)
            out.lines.pop_back();
    }

    out.source(n->e);

    if (!v.type.IsFunction()) {
        out.MakeFile();
        CompileVariable(out, p, n->variable);
        return;
    }

    PrepareCmm(out, p, n);

    out.label(v.output_name);

    CompileLevel(v.body->a);

    // The last command of the function calls function
    bool need_ret = true;
    if (!out.lines.empty()) {
        auto &b = out.lines.back();
        switch (b.opcode) {
            case AC_CALL:
                b.opcode = AC_JMP;
            case AC_PCHL:
            case AC_JMP:
            case AC_RET:
                need_ret = false;
                break;
        }
    }

    if (need_ret)
        out.ret();
}

void Cmm::Compile(CString asm_file_name) {
    // Write equs
    for (auto &vd : p.all_top_variables) {
        if (vd->address_attribute.exists) {
            out.equ(vd->output_name.c_str(), vd->address_attribute.ToString());
        } else if (!vd->only_extern && !vd->type.IsFunction() && vd->c.equ_enabled) {
            if (p.asm_names.find(vd->output_name) == p.asm_names.end())
                out.equ(vd->output_name.c_str(), vd->c.equ_text.c_str());
        }
    }

    for (CNodePtr i = p.first_node; i; i = i->next_node) {
        switch (i->type) {
            case CNT_TYPEDEF:
                break;
            case CNT_ASM:
                out.assembler(i->text);
                break;
            case CNT_DECLARE_VARIABLE:
                CompileDeclareVariable(i);
                break;
            default:
                p.Error(i->e, "can't compile node " + ToString(i->type));
        }
    }

    // Write const strings
    for (auto &i : p.const_strings)
        if (i.second->c.IsUsed())
            out.const_string(i.second->c.GetName(out.const_string_counter), i.first);

    out.MakeFile();
    out.SaveAsmFile(asm_file_name);
}

void CompileCmm(CProgramm &p, CString asm_file_name) {
    Cmm(p).Compile(asm_file_name);
}

}  // namespace I8080
