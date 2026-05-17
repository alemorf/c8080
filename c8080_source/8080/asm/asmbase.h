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

#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <inttypes.h>
#include "asmcondition.h"
#include "asmalu.h"
#include "asmregister.h"
#include "asmcommand.h"
#include "asmlabel.h"
#include "asmargument.h"
#include "../../tools/fs_tools.h"
#include "../../tools/vectorext.h"
#include "../../tools/listext.h"
#include "../../tools/number_size.h"
#include "../../c/cvariable.h"
#include "../../c/cprogramm.h"

namespace I8080 {

class AsmBase {
private:
    size_t label_counter{};
    size_t last_line_number{};

    template <typename A, typename B, typename... C>
    void Write(A &&a, B &&b, C &&...args) {
        Write(a);
        Write(b, args...);
    }

    void Write(const char *data) {
        Write2(data, strlen(data));
    }

    void Write(const CString &str) {
        Write2(str.c_str(), str.size());
    }

    void Write(uint64_t value) {
        char str[CHARS_IN_64_BIT_NUMBER + 1u]{};
        int result = snprintf(str, sizeof(str), "%" PRIu64, value);
        if (result <= 0 || result >= (int)sizeof(str))
            throw std::runtime_error(__FUNCTION__);
        Write2(str, result);
    }

    void Write(const AsmArgument &argument);

public:
    struct Line {
        AssemblerCommand opcode{};
        AsmArgument argument[2];
        AsmAlu alu{};
        AsmCondition condition{};
        const char *cursor1{};
        const char *file_name{};
    };

    CProgramm &p;
    bool measure{};
    int measure_metric{};
    uint32_t measure_regs{};
    int measure_args_metric{};
    uint32_t measure_args_regs{};
    unsigned measure_args_id{};
    std::string buffer;
    size_t prev_writePtr{};
    const char *prev_sigCursor{};
    std::vector<CVariablePtr> compile_queue;
    std::list<AsmLabel> labels;
    std::vector<Line> lines;
    uint64_t const_string_counter{};

    AsmBase(CProgramm &p_) : p(p_) {
        InitBuffer();
    }

    void InitBuffer();

    void AddHelper(Line *line, unsigned, AsmCondition condition) {
        line->condition = condition;
    }

    void AddHelper(Line *line, unsigned, AsmAlu alu) {
        line->alu = alu;
    }

    void AddHelper(Line *, unsigned) {
    }

    template <typename A, typename... C>
    void AddHelper(Line *line, unsigned n, A &&a, C &&...args) {
        line->argument[n].Set(a);
        AddHelper(line, 1, args...);
    }

    template <typename... A>
    void Add(enum AssemblerCommand command, A &&...args) {
        measure_metric += GetAsmCommandSize(command);
        if (!measure) {
            Line *l = ::Add(lines);
            l->opcode = command;
            AddHelper(l, 0, args...);
        }
    }

    void AllRegistersChanged() {
        measure_regs |= UINT32_MAX;
    }

    bool ChangedReg(AsmRegister reg);
    void AddToCompileQueue(CVariablePtr &fn);
    void MakeFile();
    void Write2(CString str);
    void Write2(const char *data, size_t size);

    void SaveAsmFile(CString file_name) {
        FsTools::SaveFile(file_name, buffer);
    }

    template <class T>
    void source(const T &e, bool function_start = false) {
        line(e.line, e.cursor, e.file_name, function_start);
    }

    void line(size_t line_number, const char *text, const char *file_name, bool function_start = false) {
        if (!measure) {
            assert(text != nullptr);
            if (line_number != last_line_number) {
                last_line_number = line_number;
                Line line{AC_LINE};
                line.cursor1 = text;
                line.file_name = file_name;
                line.argument[0].type = AAT_NUMBER;
                line.argument[0].number = line_number;
                if (function_start)
                    line.argument[1].type = AAT_NUMBER;
                lines.push_back(line);
            }
        }
    }

    AsmLabel *AllocLabel() {
        if (measure)
            return nullptr;
        // TODO: Check overflow
        // Dont' return 0
        AsmLabel *label = ::Add(labels);
        label->number = label_counter++;
        return label;
    }

    void variable(CString name) {
        Write(name);
        Write(":\n");
    }

    void align(uint64_t size) {
        Write("\t.align ", size, "\n");
    }

    void ds(size_t size) {
        if (size != 0)
            Write("\tds ", size, "\n");
    }

    void db(uint8_t value) {
        Write("\tdb ", value, "\n");
    }

    void db(CString value) {
        Write("\tdb ", value, "\n");
    }

    void dw(uint16_t value) {
        Write("\tdw ", value, "\n");
    }

    void dw(CString value) {
        Write("\tdw ", value, "\n");
    }

    void dd(uint32_t value) {
        Write("\tdd ", value, "\n");
    }

    void dd(CString value) {
        Write("\tdd ", value, "\n");
    }

    void const_string(const char *name, CString string) {
        const char *p = string.c_str();
        const char *end = p + string.size();
        Write(name, ":\n");
        unsigned n = 0;
        for (;;) {
            Write(n == 0 ? "  db " : ", ");
            Write(static_cast<unsigned char>(*p));
            if (p == end)
                break;
            p++;
            n++;
            if (n == 16) {
                Write("\n");
                n = 0;
            }
        }
        Write("\n");
    }

    void equ(CString name, CString string) {
        Write(name, " equ ", string, "\n");
    }
};

}  // namespace I8080
