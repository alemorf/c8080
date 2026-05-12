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

#include <stdint.h>
#include "../Compile.h"
#include "../../c/cprogramm.h"
#include "../../c/tools/cthrow.h"
#include "../asm/asm.h"
#include "../InternalFunctions.h"

class CParser;  // todo: remove

namespace I8080 {

class Compiler {
protected:
    CProgramm &p;
    Asm out;
    AsmLabel *return_label{};
    AsmLabel *break_label{};
    AsmLabel *continue_label{};
    CVariablePtr current_function;
    CVariablePtr static_stack;
    std::vector<CVariablePtr> call_in_call;
    InternalFunctions o;
    CBuildProc measure_proc{};

    void CompileCommand(CNodePtr &node);

    void CompileSwitch(CNodePtr &node);
    void CompileSwitch8(CNodePtr &node, std::vector<CNodePtr> &cases, AsmLabel *label);
    void CompileSwitch16(CNodePtr &node, std::vector<CNodePtr> &cases, AsmLabel *label);

    AsmRegister GetResultReg(CType &type, bool alt, bool no_result, CNodePtr &e);

    void BuildArgs2(const CNodePtr &node, AsmRegister reg, CNodePtr &a, CNodePtr &b, AsmRegister main, AsmRegister alt,
                    bool can_swap);
    bool Case_Args2_MM(CNodePtr &a, CNodePtr &b, AsmRegister main, AsmRegister alt, unsigned bam);
    bool Case_Args2_MA(CNodePtr &a, CNodePtr &b, AsmRegister main, AsmRegister alt, unsigned bam);
    bool Case_Args2_AM(CNodePtr &a, CNodePtr &b, AsmRegister main, AsmRegister alt, unsigned bam);

    void Build(CNodePtr &node, AsmRegister reg = REG_PREPARE);

    uint32_t BuildJumpIfZero(bool prepare, CNodePtr &node, bool jmp_if_not_zero, bool invert, AsmLabel *label);

    void BuildJumpIf(bool prepare, CNodePtr &node, bool jmp_if_true, AsmLabel *label);

    void BuildOperator(CNodePtr &node);
    bool Case_If(CNodePtr &node, AsmRegister reg);
    bool Case_Comma(CNodePtr &node, AsmRegister reg);
    bool Case_StackAddress(CNodePtr &node, AsmRegister reg);
    bool Case_ArgStackAddress(CNodePtr &node, AsmRegister reg);
    bool Case_JumpNode(CNodePtr &node, AsmRegister reg);

    void BuildCall(CNodePtr &node);
    bool Case_Call(CNodePtr &node, AsmRegister reg);
    void InternalCall(CVariablePtr &fn);
    void MakeCallTreeBegin(CVariablePtr &fn);
    void MakeCallTreeEnd();

    bool BuildDouble(CNodePtr &node, AsmRegister reg, CNodePtr &a, AsmRegister a_reg, CNodePtr &b, AsmRegister b_reg);
    void Case_Double(CNodePtr &a, AsmRegister a_reg, CNodePtr &b, AsmRegister br, CBuildCase &bc);

    void BuildOperator32(CNodePtr &node);
    bool Case_Operator32(CNodePtr &node, AsmRegister reg);

    void BuildMonoOperator(CNodePtr &node);
    bool Case_Minus8(CNodePtr &node, AsmRegister reg);
    bool Case_Minus16(CNodePtr &node, AsmRegister reg);
    bool Case_Minus32(CNodePtr &node, AsmRegister reg);
    bool Case_Neg8(CNodePtr &node, AsmRegister reg);
    bool Case_Neg16(CNodePtr &node, AsmRegister reg);
    bool Case_Neg32(CNodePtr &node, AsmRegister reg);

    void BuildConst(CNodePtr &node);
    bool Case_Const0(CNodePtr &node, AsmRegister reg);
    bool Case_Const8(CNodePtr &node, AsmRegister reg);
    bool Case_Const16(CNodePtr &node, AsmRegister reg);
    bool Case_Const32(CNodePtr &node, AsmRegister reg);

    void BuildLoad(CNodePtr &node);
    bool Case_Empty(CNodePtr &node, AsmRegister reg);
    bool Case_Direct(CNodePtr &node, AsmRegister reg);
    bool Case_LoadConstAddr_8(CNodePtr &node, AsmRegister reg);
    bool Case_LoadConstAddr_16(CNodePtr &node, AsmRegister reg);
    bool Case_LoadConstAddr_32(CNodePtr &node, AsmRegister reg);
    bool Case_Load_8(CNodePtr &node, AsmRegister reg);
    bool Case_Load_8_A(CNodePtr &node, AsmRegister reg);
    bool Case_Load_16(CNodePtr &node, AsmRegister reg);
    bool Case_Load_32(CNodePtr &node, AsmRegister reg);

    void BuildConvert(CNodePtr &node);
    bool Case_Convert_void(CNodePtr &node, AsmRegister reg);
    bool Case_ConvertLoadConstAddr_8(CNodePtr &node, AsmRegister reg);
    bool Case_ConvertLoad_8(CNodePtr &node, AsmRegister reg);
    bool Case_ConvertLoad_8_A(CNodePtr &node, AsmRegister reg);
    bool Case_ConvertLoadConstAddr_16(CNodePtr &node, AsmRegister reg);
    bool Case_ConvertLoad_16(CNodePtr &node, AsmRegister reg);
    bool Case_ConvertLoadConstAddr_U8_16(CNodePtr &node, AsmRegister reg);
    bool Case_ConvertLoad_U8_16(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_U8_16_MM(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_U8_16_AM(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_U8_16_MA(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_U8_16_AA(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_S8_16(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_U8_32_M(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_U8_32_A(CNodePtr &node, AsmRegister reg);
    bool Case_ConvertLoadConstAddr_U8_32(CNodePtr &node, AsmRegister reg);
    bool Case_ConvertLoad_U8_32(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_S8_32(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_U16_32(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_S16_32(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_16_8_M(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_16_8_A(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_32_8(CNodePtr &node, AsmRegister reg);
    bool Case_Convert_32_16(CNodePtr &node, AsmRegister reg);

    void MeasureBegin(AsmRegister reg);
    void MeasureBegin();
    void MeasureResult(CNodePtr &node, AsmRegister reg);
    bool MeasureReset(CNodePtr &node, AsmRegister reg);
    void Measure(CNodePtr &node, AsmRegister reg, CBuildProc proc);
    void MeasureArgsBegin();
    void MeasureArgsEnd(unsigned id);

    void BuildOperator8(CNodePtr &node);
    bool Case_Operator8(CNodePtr &node, AsmRegister reg);
    bool Case_Operator8_PMR(CNodePtr &node, AsmRegister reg);
    bool Case_Operator8_PM(CNodePtr &node, AsmRegister reg);
    bool Case_Operator8_MCR(CNodePtr &node, AsmRegister reg);
    bool Case_Operator8_MC(CNodePtr &node, AsmRegister reg);
    bool Case_Mul8_MCR(CNodePtr &node, AsmRegister reg);
    bool Case_Mul8_AC(CNodePtr &node, AsmRegister reg);
    bool Case_Mul8_ACR(CNodePtr &node, AsmRegister reg);
    bool Case_Mul8_MC(CNodePtr &node, AsmRegister reg);
    bool Case_Shl8_MC(CNodePtr &node, AsmRegister reg);
    bool Case_Shr8_MC(CNodePtr &node, AsmRegister reg);
    AsmAlu OperatorCodeToAlu(COperatorCode code);
    void Alu8D(CNodePtr &node);

    void BuildOperator16(CNodePtr &node);
    bool Case_Operator16(CNodePtr &node, AsmRegister reg);
    bool Case_Shl16_MN(CNodePtr &node, AsmRegister reg);
    bool Case_Shr16_MN(CNodePtr &node, AsmRegister reg);
    bool Case_IncDec16_N(CNodePtr &node, AsmRegister reg);
    bool Case_Mul16_MC(CNodePtr &node, AsmRegister reg);
    bool Case_Mul16_AC(CNodePtr &node, AsmRegister reg);
    bool Case_Mul16_MCR(CNodePtr &node, AsmRegister reg);
    bool Case_Mul16_ACR(CNodePtr &node, AsmRegister reg);
    void Alu16(CNodePtr &node);

    void BuildSet(CNodePtr &node);
    bool Case_Set8_MM(CNodePtr &node, AsmRegister reg);
    bool Case_Set8_AM(CNodePtr &node, AsmRegister reg);
    bool Case_Set8_MA(CNodePtr &node, AsmRegister reg);
    bool Case_Set8_MNR(CNodePtr &node, AsmRegister reg);
    bool Case_Set8_MN(CNodePtr &node, AsmRegister reg);
    bool Case_Set16_AA(CNodePtr &node, AsmRegister reg);
    bool Case_Set16_NM(CNodePtr &node, AsmRegister reg);
    bool Case_Set32_MM(CNodePtr &node, AsmRegister reg);
    bool Case_Set32_AM(CNodePtr &node, AsmRegister reg);
    bool Case_Set32_NM(CNodePtr &node, AsmRegister reg);

    void OutSubSpN(size_t value);
    void OutAddSpN(AsmRegister save, size_t value);
    bool OutIncDec16(AsmRegister reg, CNodePtr &a, CNodePtr &b);
    void OutMul16(CNodePtr &var, uint16_t value, AsmRegister reg);
    void OutMul8(CNodePtr &var, CNodePtr &number, AsmRegister reg);
    void OutShr8(CNodePtr &node, AsmRegister reg);
    void OutShl8(CNodePtr &node, AsmRegister reg);

public:
    Compiler(CProgramm &p_) : p(p_), out(p_) {
    }

    void Compile(CParser &c, OutputFormat output_format, CString output_file_bin, CString asm_file_name);
};

}  // namespace I8080
