/*
 * c8080 compiler
 * Copyright (c) 2022 Aleksey Morozov
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

#include "asmbase.h"
#include <string>
#include <map>
#include <assert.h>
#include <math.h>

namespace I8080 {

void AsmBase::InitBuffer() {
    buffer = "    device zxspectrum48 ; There is no ZX Spectrum, it is needed for the sjasmplus assembler.\n";
}

void AsmBase::AddToCompileQueue(CVariablePtr &fn) {
    if (fn->c.use_counter == 0) {
        fn->c.use_counter++;

        if (!fn->only_extern)
            compile_queue.push_back(fn);
    }
}

bool AsmBase::ChangedReg(AsmRegister reg) {
    if (!measure)
        return true;
    measure_regs |= RegToUsed(reg);
    return false;
}

void AsmBase::Write2(CString str) {
    Write2(str.c_str(), str.size());
}

void AsmBase::Write2(const char *data, size_t size) {
    size_t newSize = buffer.size() + size;  //! Тут может быть переполнение
    if (newSize > buffer.capacity()) {
        size_t gran = buffer.capacity() + buffer.capacity() / 2;  //! Тут может быть переполнение
        if (newSize < gran)
            newSize = gran;
        buffer.reserve(newSize);
    }
    buffer.append(data, size);
}

void AsmBase::Write(const AsmArgument &argument) {
    switch (argument.type) {
        case AAT_STRING:
            if (argument.string.empty())
                throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Empty string");
            Write(argument.string.c_str());
            break;
        case AAT_NUMBER:
            Write(argument.number);
            break;
        case AAT_LABEL:
            Write("__l_");
            if (argument.label == nullptr)
                throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " Incorrect label");
            Write(argument.label->number);
            break;
        case AAT_REG:
            Write(ToString(argument.reg));
            break;
        default:
            throw std::runtime_error(std::string(__PRETTY_FUNCTION__) +
                                     " type = " + std::to_string(static_cast<size_t>(argument.type)));
    }
}

void AsmBase::MakeFile() {
#if 1
    const char *cursor = nullptr;
    const char *file_name = nullptr;
    size_t line_number = 0;
#endif
    uint16_t in_stack = 0;
    uint16_t args_stack_offset = 0;
    for (auto &line : lines) {
        switch (line.opcode) {
            case AC_LINE: {
                assert(line.cursor1 != nullptr);
                assert(line.argument[0].type == AAT_NUMBER);
#if 0
                Write("; ");
                Write(line.argument[0].number);
                Write(" ");
                const char* text = line.cursor1 - (line.argument[1].number - 1);
                const char* line_end = strchr(text, '\n');
                if (line_end == nullptr)
                    Write(text);
                else
                    Write(text, line_end - text);
                Write("\n");
#else
                if (line.argument[1].type == AAT_NUMBER || cursor == nullptr || file_name != line.file_name) {
                    file_name = line.file_name;
                    cursor = line.cursor1;
                    line_number = line.argument[0].number;
                }
                while ((line_number <= line.argument[0].number) && (cursor[0] != '\0')) {
                    Write("; ", line_number, " ");
                    line_number++;
                    const char *line_end = strchr(cursor, '\n');
                    if (line_end == nullptr) {
                        Write(cursor, "\n");
                        cursor = "";
                        break;
                    }
                    Write2(cursor, line_end - cursor);
                    cursor = line_end + 1u;
                    Write("\n");
                }
#endif
                break;
            }
            case AC_REMARK:
                Write("; ", line.argument[0], "\n");
                break;
            case AC_STACK_CORRECTION:
                assert(line.argument[0].type == AAT_NUMBER);
                in_stack += static_cast<int16_t>(line.argument[0].number);
                break;
            case AC_STACK_CORRECTION_RESET:
                assert(line.argument[0].type == AAT_NUMBER);
                args_stack_offset = line.argument[0].number;
                in_stack = 0;
                break;
            case AC_LXI_STACK_ADDR:
                assert(line.argument[0].type == AAT_REG);
                assert(line.argument[1].type == AAT_NUMBER);
                Write("\tld ", line.argument[0], ", ", line.argument[1].number + in_stack, "\n");
                break;
            case AC_LXI_ARG_STACK_ADDR:
                assert(line.argument[0].type == AAT_REG);
                assert(line.argument[1].type == AAT_NUMBER);
                Write("\tld ", line.argument[0], ", ", line.argument[1].number + in_stack + args_stack_offset, "\n");
                break;
            case AC_LXI:
            case AC_MVI:
                assert(line.argument[0].type == AAT_REG);
                Write("\tld ", line.argument[0], ", ", line.argument[1], "\n");
                break;
            case AC_SHLD:
                Write("\tld (", line.argument[0], "), hl\n");
                break;
            case AC_LHLD:
                Write("\tld hl, (", line.argument[0], ")\n");
                break;
            case AC_STA:
                Write("\tld (", line.argument[0], "), a\n");
                break;
            case AC_LDA:
                Write("\tld a, (", line.argument[0], ")\n");
                break;
            case AC_MOV:
                Write("\tld ", line.argument[0], ", ", line.argument[1], "\n");
                break;
            case AC_XCHG:
                Write("\tex hl, de\n");
                break;
            case AC_XTHL:
                Write("\tex (sp), hl\n");
                break;
            case AC_RAL:
                Write("\trla\n");
                break;
            case AC_RAR:
                Write("\trra\n");
                break;
            case AC_RRC:
                Write("\trrca\n");
                break;
            case AC_RLC:
                Write("\trlca\n");
                break;
            case AC_JMP:
                Write("\tjp ", line.argument[0], "\n");
                break;
            case AC_JMP_CONDITION:
                Write("\tjp ", ToString(line.condition), ", ", line.argument[0], "\n");
                break;
            case AC_CALL_CONDITION:
                Write("\tcall ", ToString(line.condition), ", ", line.argument[0], "\n");
                break;
            case AC_RET_CONDITION:
                Write("\tret ", ToString(line.condition), "\n");
                break;
            case AC_LABEL:
                Write(line.argument[0], ":\n");
                break;
            case AC_PUSH:
                in_stack += 2;
                if (line.argument[0].type == AAT_REG && line.argument[0].reg == R8_A)
                    Write("\tpush af\n");
                else
                    Write("\tpush ", line.argument[0], "\n");
                break;
            case AC_POP:
                in_stack -= 2;
                if (line.argument[0].type == AAT_REG && line.argument[0].reg == R8_A)
                    Write("\tpop af\n");
                else
                    Write("\tpop ", line.argument[0], "\n");
                break;
            case AC_RET:
                // TODO: assert
                Write("\tret\n");
                break;
            case AC_CALL:
                Write("\tcall ", line.argument[0], "\n");
                break;
            case AC_ALU_CONST:
                Write("\t", ToString(line.alu), " ", line.argument[0], "\n");
                break;
            case AC_ALU_REG:
                Write("\t", ToString(line.alu), " ", line.argument[0], "\n");
                break;
            case AC_DAD:
                Write("\tadd hl, ", line.argument[0], "\n");
                break;
            case AC_SPHL:
                Write("\tld sp, hl\n");
                break;
            case AC_CMA:
                Write("\tcpl\n");
                break;
            case AC_INC:
                Write("\tinc ", line.argument[0], "\n");
                if (line.argument[0].type == AAT_REG && line.argument[0].reg == R16_SP)
                    in_stack--;
                break;
            case AC_DEC:
                Write("\tdec ", line.argument[0], "\n");
                if (line.argument[0].type == AAT_REG && line.argument[0].reg == R16_SP)
                    in_stack++;
                break;
            case AC_LDAX:
                Write("\tld a, (", line.argument[0], ")\n");
                break;
            case AC_STAX:
                Write("\tld (", line.argument[0], "), a\n");
                break;
            case AC_IN:
                Write("\tin a, (", line.argument[0], ")\n");
                break;
            case AC_OUT:
                Write("\tout (", line.argument[0], "), a\n");
                break;
            case AC_PCHL:
                Write("\tjp hl\n");
                break;
            case AC_STC:
                Write("\tscf\n");
                break;
            case AC_ASSEMBLER:
                Write(line.argument[0], "\n");
                break;
            case AC_REMOVED:
                break;
            case AC_EI:
                Write("\tei\n");
                break;
            case AC_DI:
                Write("\tdi\n");
                break;
            case AC_DAA:
                Write("\tdaa\n");
                break;
            case AC_NOP:
                Write("\tnop\n");
                break;
            default:
                throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " " + std::to_string(line.opcode));
        }
    }
    lines.clear();
}

}  // namespace I8080
