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

#include "asmbase.h"
#include "../../c/cnode.h"
#include "../../c/tools/getnumberasuint64.h"
#include "../../c/cprogramm.h"

namespace I8080 {

class Asm : public AsmBase {
public:
    static const unsigned CALL_METRICS = 100;

    Asm(CProgramm &p_) : AsmBase(p_) {
    }

    std::string GetConst(const CNodePtr &node, bool *error = nullptr, std::vector<CVariablePtr> *use = nullptr);

    void ld_pstring_hl(CString string) {
        Add(AC_SHLD, string);
    }

    void ld_pnumber_hl(uint16_t number) {
        Add(AC_SHLD, number);
    }

    void ld_pconst_hl(const CNodePtr &node) {
        if (node->type == CNT_NUMBER)
            ld_pnumber_hl(GetNumberAsUint64(node));
        else
            ld_pstring_hl(GetConst(node));
    }

    void ld_pconst_dehl_xchg(const CNodePtr &node) {
        if (node->type == CNT_NUMBER) {
            uint32_t n = GetNumberAsUint64(node);
            ld_pnumber_hl(n);
            ex_hl_de();
            ld_pnumber_hl(n >> 16);
        } else {
            std::string s = GetConst(node);
            ld_pstring_hl(s);
            ex_hl_de();
            ld_pstring_hl(s + " + 2");
        }
    }

    void ld_a_pstring(CString string) {
        Add(AC_LDA, string);
    }

    void ld_a_pnumber(uint16_t number) {
        Add(AC_LDA, number);
    }

    void ld_a_pconst(const CNodePtr &node) {
        if (node->type == CNT_NUMBER)
            ld_a_pnumber(GetNumberAsUint64(node));
        else
            ld_a_pstring(GetConst(node));
    }

    void ex_hl_de() {
        ChangedReg(R32_DEHL);
        Add(AC_XCHG);
    }

    void ex_psp_hl() {
        ChangedReg(R16_HL);
        Add(AC_XTHL);
    }

    void push_reg(AsmRegister reg) {
        Add(AC_PUSH, reg);
    }

    void pop_reg(AsmRegister reg) {
        ChangedReg(reg);
        Add(AC_POP, reg);
    }

    // ┌─<─ CF <─ D7 .. D0 <──┐
    // └──────────────────────┘

    void carry_rotate_left() {
        ChangedReg(R8_A);
        Add(AC_RAL);
    }

    // ┌─>─ CF >─ D7 .. D0 >──┐
    // └──────────────────────┘

    void carry_rotate_right() {
        ChangedReg(R8_A);
        Add(AC_RAR);
    }

    // ┌─> D7 .. D0 >─┬─> CF
    // └──────────────┘

    void cyclic_rotate_right() {
        ChangedReg(R8_A);
        Add(AC_RRC);
    }

    // CF <─┬─< D7 .. D0 <─┐
    //      └──────────────┘

    void cyclic_rotate_left() {
        ChangedReg(R8_A);
        Add(AC_RLC);
    }

    void ei() {
        Add(AC_EI);
    }

    void di() {
        Add(AC_DI);
    }

    void daa() {
        Add(AC_DAA);
    }

    void nop() {
        Add(AC_NOP);
    }

    void pchl() {
        Add(AC_PCHL);
    }

    void stc() {
        Add(AC_STC);
    }

    void ld_sp_hl() {
        ChangedReg(R16_SP);
        Add(AC_SPHL);
    }

    void cpl() {
        ChangedReg(R16_AF);
        Add(AC_CMA);
    }

    void call(CString string) {
        AllRegistersChanged();
        Add(AC_CALL, string);
    }

    void call(uint16_t number) {
        AllRegistersChanged();
        Add(AC_CALL, number);
    }

    void ld_pstring1_a(CString string) {
        Add(AC_STA, string);
    }

    void ld_pnumber_a(uint16_t number) {
        Add(AC_STA, number);
    }

    void ld_pconst_a(const CNodePtr &node) {
        if (node->type == CNT_NUMBER)
            ld_pnumber_a(GetNumberAsUint64(node));
        else
            ld_pstring1_a(GetConst(node));
    }

    void rotate(AssemblerCommand opcode) {
        assert(opcode == AC_RAL || opcode == AC_RAR || opcode == AC_RLC || opcode == AC_RRC);
        ChangedReg(R8_A);
        Add(opcode);
    }

    void dec_reg(AsmRegister reg) {
        ChangedReg(reg);
        Add(AC_DEC, reg);
    }

    void inc_reg(AsmRegister reg) {
        ChangedReg(reg);
        Add(AC_INC, reg);
    }

    void in(CString string) {
        ChangedReg(R8_A);
        Add(AC_IN, string);
    }

    void out(CString string) {
        Add(AC_OUT, string);
    }

    void Remark(CString string) {
        Add(AC_REMARK, string);
    }

    void label(CString string) {
        Add(AC_LABEL, string);
    }

    void label(AsmLabel *label) {
        Add(AC_LABEL, label);
        if (!measure)
            label->destination = lines.size();
    }

    void jmp_label(AsmLabel *label) {
        Add(AC_JMP, label);
    }

    void jmp(CString string) {
        Add(AC_JMP, string);
    }

    void ld_hl_pstring(CString string) {
        ChangedReg(R16_HL);
        Add(AC_LHLD, string);
    }

    void ld_hl_pnumber(uint16_t number) {
        ChangedReg(R16_HL);
        Add(AC_LHLD, number);
    }

    void ld_hl_pconst(const CNodePtr &node) {
        if (node->type == CNT_NUMBER)
            ld_hl_pnumber(GetNumberAsUint64(node));
        else
            ld_hl_pstring(GetConst(node));
    }

    void ld_dehl_pstring(CString string) {
        ld_hl_pstring("((" + string + ") + 2)");
        ex_hl_de();
        ld_hl_pstring(string);
    }

    void ld_dehl_pnumber(uint32_t number) {
        ld_hl_pnumber(number + 2);
        ex_hl_de();
        ld_hl_pnumber(number);
    }

    void ld_dehl_pconst(const CNodePtr &node) {
        if (node->type == CNT_NUMBER)
            ld_dehl_pnumber(GetNumberAsUint64(node));
        else
            ld_dehl_pstring(GetConst(node));
    }

    void add_hl_reg(AsmRegister reg) {
        assert(reg == R16_BC || reg == R16_DE || reg == R16_HL || reg == R16_SP);
        Add(AC_DAD, reg);
    }

    void assembler(CString string) {
        AllRegistersChanged();
        Add(AC_ASSEMBLER, string);
    }

    void ret() {
        Add(AC_RET);
    }

    void ld_a_preg(AsmRegister reg) {
        switch (reg) {
            case R16_HL:
                ld_r8_r8(R8_A, R8_M);
                break;
            case R16_BC:
            case R16_DE:
                ChangedReg(R8_A);
                Add(AC_LDAX, reg);
                break;
            default:
                throw std::runtime_error(__PRETTY_FUNCTION__);
        }
    }

    void ld_r8_r8(AsmRegister reg_a, AsmRegister reg_b) {
        assert(reg_a != R8_M || reg_b != R8_M);
        ChangedReg(reg_a);
        Add(AC_MOV, reg_a, reg_b);
    }

    void ld_reg_phl(AsmRegister reg) {
        assert(reg != R8_M);
        ChangedReg(reg);
        Add(AC_MOV, reg, R8_M);
    }

    void ld_preg_a(AsmRegister reg) {
        switch (reg) {
            case R16_HL:
                ld_r8_r8(R8_M, R8_A);
                break;
            case R16_BC:
            case R16_DE:
                Add(AC_STAX, reg);
                break;
            default:
                throw std::runtime_error(__PRETTY_FUNCTION__);
        }
    }

    void stack_correction_reset(uint16_t args_offset) {
        Add(AC_STACK_CORRECTION_RESET, args_offset);
    }

    void stack_correction(int16_t value) {
        Add(AC_STACK_CORRECTION, uint16_t(value));
    }

    void ld_reg_stack_addr(AsmRegister reg, uint16_t offset) {
        assert(reg == R16_HL || reg == R16_DE || reg == R16_BC);
        ChangedReg(reg);
        Add(AC_LXI_STACK_ADDR, reg, offset);
    }

    void ld_reg_arg_stack_addr(AsmRegister reg, uint16_t offset) {
        assert(reg == R16_HL || reg == R16_DE || reg == R16_BC);
        ChangedReg(reg);
        Add(AC_LXI_ARG_STACK_ADDR, reg, offset);
    }

    void alu_a_reg(AsmAlu alu, AsmRegister reg) {
        ChangedReg(R8_A);
        Add(AC_ALU_REG, reg, alu);
    }

    void alu_a_string(AsmAlu alu, CString string) {
        ChangedReg(R8_A);
        Add(AC_ALU_CONST, string, alu);
    }

    void alu_a_number(AsmAlu alu, uint8_t number) {
        if ((number == 1 && alu == ALU_ADD) || (number == 0xFF && alu == ALU_SUB)) {
            inc_a();
            return;
        }
        if ((number == 1 && alu == ALU_SUB) || (number == 0xFF && alu == ALU_ADD)) {
            dec_a();
            return;
        }
        ChangedReg(R8_A);
        Add(AC_ALU_CONST, number, alu);
    }

    void alu_a_const(AsmAlu alu, const CNodePtr &number) {
        if (number->type == CNT_NUMBER)
            alu_a_number(alu, GetNumberAsUint64(number));
        else
            alu_a_string(alu, GetConst(number));
    }

    void ld_r8_number(AsmRegister reg, uint8_t number) {
        switch (reg) {
            case R8_A:
            case R8_B:
            case R8_C:
            case R8_D:
            case R8_E:
            case R8_H:
            case R8_L:
            case R8_M:
                number &= 0xFFu;
                break;
        }
        if (reg == R8_A && number == 0) {
            alu_a_reg(ALU_XOR, R8_A);
        } else {
            ChangedReg(reg);
            Add(AC_MVI, reg, number);
        }
    }

    void ld_r16_number(AsmRegister reg, uint16_t number) {
        ChangedReg(reg);
        Add(AC_LXI, reg, number);
    }

    void ld_r8_string(AsmRegister reg, CString string) {
        ChangedReg(reg);
        Add(AC_MVI, reg, string);
    }

    void ld_r16_string(AsmRegister reg, CString string) {
        ChangedReg(reg);
        Add(AC_LXI, reg, string);
    }

    void ld_r8_const(AsmRegister reg, const CNodePtr &node) {  // can xor
        if (node->type == CNT_NUMBER)
            ld_r8_number(reg, GetNumberAsUint64(node));
        else
            ld_r8_string(reg, GetConst(node));
    }

    void ld_r16_const(AsmRegister reg, const CNodePtr &node) {
        ChangedReg(reg);
        if (node->type == CNT_NUMBER)
            Add(AC_LXI, reg, GetNumberAsUint64(node));
        else
            Add(AC_LXI, reg, GetConst(node));
    }

    void ld_dehl_const(const CNodePtr &node) {
        if (node->type == CNT_NUMBER) {
            const uint64_t value = GetNumberAsUint64(node);
            ld_r16_number(R16_DE, value >> 16);
            ld_r16_number(R16_HL, value);
        } else {
            const std::string value = GetConst(node);
            ld_r16_string(R16_DE, "(" + value + ") >> 16");
            ld_r16_string(R16_HL, "(" + value + ") & 0FFFFh");
        }
    }

    void ret_condition(AsmCondition condition) {
        Add(AC_RET_CONDITION, condition);
    }

    void jmp_condition(AsmCondition condition, CString name) {
        Add(AC_JMP_CONDITION, name, condition);
    }

    void jmp_condition_label(AsmCondition condition, AsmLabel *label) {
        Add(AC_JMP_CONDITION, label, condition);
    }

    void call_condition(AsmCondition condition, CString name) {
        AllRegistersChanged();
        Add(AC_CALL_CONDITION, name, condition);
    }

    // Simple commands

    void ld_de_number(uint16_t number) {
        ld_r16_number(R16_DE, number);
    }

    void ld_hl_number(uint16_t number) {
        ld_r16_number(R16_HL, number);
    }

    void ld_a_pde() {
        ld_a_preg(R16_DE);
    }

    void ld_a_phl() {
        ld_a_preg(R16_HL);
    }

    void ld_l_phl() {
        ld_r8_r8(R8_L, R8_M);
    }

    void ld_d_phl() {
        ld_r8_r8(R8_D, R8_M);
    }

    void ld_e_phl() {
        ld_r8_r8(R8_E, R8_M);
    }

    void ld_phl_a() {
        ld_r8_r8(R8_M, R8_A);
    }

    void ld_pde_a() {
        ld_preg_a(R16_DE);
    }

    void ld_l_a() {
        ld_r8_r8(R8_L, R8_A);
    }

    void ld_d_a() {
        ld_r8_r8(R8_D, R8_A);
    }

    void ld_a_d() {
        ld_r8_r8(R8_A, R8_D);
    }

    void ld_a_l() {
        ld_r8_r8(R8_A, R8_L);
    }

    void ld_a_h() {
        ld_r8_r8(R8_A, R8_H);
    }

    void ld_h_d() {
        ld_r8_r8(R8_H, R8_D);
    }

    void ld_h_l() {
        ld_r8_r8(R8_H, R8_L);
    }

    void ld_a_n8(uint8_t value) {
        ld_r8_number(R8_A, value);
    }

    void ld_d_number(uint8_t value) {
        ld_r8_number(R8_D, value);
    }

    void jz_label(AsmLabel *label) {
        jmp_condition_label(JC_Z, label);
    }

    void jnz_label(AsmLabel *label) {
        jmp_condition_label(JC_NZ, label);
    }

    void jc_label(AsmLabel *label) {
        jmp_condition_label(JC_C, label);
    }

    void jnc_label(AsmLabel *label) {
        jmp_condition_label(JC_NC, label);
    }

    void jm_label(AsmLabel *label) {
        jmp_condition_label(JC_M, label);
    }

    void jp_label(AsmLabel *label) {
        jmp_condition_label(JC_P, label);
    }

    void push_af() {
        push_reg(R16_AF);
    }

    void pop_af() {
        pop_reg(R16_AF);
    }

    void push_hl() {
        push_reg(R16_HL);
    }

    void push_de() {
        push_reg(R16_DE);
    }

    void pop_hl() {
        pop_reg(R16_HL);
    }

    void pop_bc() {
        pop_reg(R16_BC);
    }

    void pop_de() {
        pop_reg(R16_DE);
    }

    void ld_phl_e() {
        ld_r8_r8(R8_M, R8_E);
    }

    void ld_phl_d() {
        ld_r8_r8(R8_M, R8_D);
    }

    void ld_phl_reg(AsmRegister reg) {
        ld_r8_r8(R8_M, reg);
    }

    void ld_phl_string(CString value) {
        ld_r8_string(R8_M, value);
    }

    void ld_phl_number(uint8_t value) {
        ld_r8_number(R8_M, value);
    }

    void ld_phl_const(const CNodePtr &node) {
        if (node->type == CNT_NUMBER)
            ld_phl_number(GetNumberAsUint64(node));
        else
            ld_phl_string(GetConst(node));
    }

    void inc_de() {
        inc_reg(R16_DE);
    }

    void inc_hl() {
        inc_reg(R16_HL);
    }

    void dec_de() {
        dec_reg(R16_DE);
    }

    void dec_hl() {
        dec_reg(R16_HL);
    }

    void inc_phl() {
        inc_reg(R8_M);
    }

    void dec_phl() {
        dec_reg(R8_M);
    }

    void cmp_a_d() {
        alu_a_reg(ALU_CMP, R8_D);
    }

    void cmp_a_phl() {
        alu_a_reg(ALU_CMP, R8_M);
    }

    void cmp_string(CString string) {
        alu_a_string(ALU_CMP, string);
    }

    void add_string(CString string) {
        alu_a_string(ALU_ADD, string);
    }

    void sub_string(CString string) {
        alu_a_string(ALU_SUB, string);
    }

    void and_string(CString string) {
        alu_a_string(ALU_AND, string);
    }

    void and_number(uint8_t number) {
        alu_a_number(ALU_AND, number);
    }

    void or_string(CString string) {
        alu_a_string(ALU_OR, string);
    }

    void xor_string(CString string) {
        alu_a_string(ALU_XOR, string);
    }

    void ld_de_hl() {
        ld_r8_r8(R8_D, R8_H);
        ld_r8_r8(R8_E, R8_L);
    }

    void inc_a() {
        inc_reg(R8_A);
    }

    void dec_a() {
        dec_reg(R8_A);
    }

    void push_de_hl() {
        push_reg(R16_DE);
        push_reg(R16_HL);
    }

    void pop_hl_de() {
        pop_reg(R16_HL);
        pop_reg(R16_DE);
    }

    void ld_hl_bc() {
        ld_r8_r8(R8_H, R8_B);
        ld_r8_r8(R8_L, R8_C);
    }

    void ld_bc_hl() {
        ld_r8_r8(R8_B, R8_H);
        ld_r8_r8(R8_C, R8_L);
    }

    void add_d() {
        alu_a_reg(ALU_ADD, R8_D);
    }

    void add_a() {
        alu_a_reg(ALU_ADD, R8_A);
    }

    void sub_a_d() {
        alu_a_reg(ALU_SUB, R8_D);
    }

    void and_a_d() {
        alu_a_reg(ALU_AND, R8_D);
    }

    void or_d() {
        alu_a_reg(ALU_OR, R8_D);
    }

    void or_e() {
        alu_a_reg(ALU_OR, R8_E);
    }

    void or_l() {
        alu_a_reg(ALU_OR, R8_L);
    }

    void xor_a_d() {
        alu_a_reg(ALU_XOR, R8_D);
    }

    void add_hl_de() {
        add_hl_reg(R16_DE);
    }

    void add_hl_hl() {
        add_hl_reg(R16_HL);
    }

    void add_hl_sp() {
        add_hl_reg(R16_SP);
    }

    void or_a() {
        alu_a_reg(ALU_OR, R8_A);
    }

    void or_h() {
        alu_a_reg(ALU_OR, R8_H);
    }

    void ld_h_number(uint8_t number) {
        ld_r8_number(R8_H, number);
    }

    void ld_l_number(uint8_t number) {
        ld_r8_number(R8_L, number);
    }

    void ld_de_offset(CString string) {
        ld_r16_string(R16_DE, string);
    }

    void ld_hl_offset(CString string) {
        ld_r16_string(R16_HL, string);
    }

    unsigned data(CNodePtr &node) {
        if (node->type == CNT_NUMBER) {
            switch (node->ctype.GetAsmType()) {
                case CBT_CHAR:
                    db(node->number.i);
                    return 1;
                case CBT_UNSIGNED_CHAR:
                    db(node->number.u);
                    return 1;
                case CBT_SHORT:
                    dw(node->number.i);
                    return 2;
                case CBT_UNSIGNED_SHORT:
                    dw(node->number.u);
                    return 2;
                case CBT_LONG:
                    dd(node->number.i);
                    return 4;
                case CBT_UNSIGNED_LONG:
                    dd(node->number.u);
                    return 4;
                default:
                    C_ERROR_UNSUPPORTED_ASM_TYPE(node);
            }
        }
        switch (node->ctype.GetAsmType()) {
            case CBT_CHAR:
            case CBT_UNSIGNED_CHAR:
                db(GetConst(node));
                return 1;
            case CBT_SHORT:
            case CBT_UNSIGNED_SHORT:
                dw(GetConst(node));
                return 2;
            case CBT_LONG:
            case CBT_UNSIGNED_LONG:
                dd(GetConst(node));
                return 4;
            default:
                C_ERROR_UNSUPPORTED_ASM_TYPE(node);
        }
        return 0;
    }
};

}  // namespace I8080
