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

#include "compiler/Compiler.h"
#include <assert.h>

namespace I8080 {

bool ParseOutputFormat(enum OutputFormat &result, CString str) {
    if (0 == str.CaseCmp("i1080")) {
        result = OF_I1080;
        return true;
    }
    if (0 == str.CaseCmp("cpm")) {
        result = OF_CPM;
        return true;
    }
    if (0 == str.CaseCmp("rks")) {
        result = OF_RKS;
        return true;
    }
    return false;
}

void RegisterProhibitedOutputNames(CProgramm &programm) {
    static const char *prohibited_output_names[] = {
        // These names conflicts with static headers

        "__begin",
        "__entry",
        "__bss",
        "__end",

        // These names conflicts with the assembler
        // Example:
        //      ld a, (a)
        //   a: db 10

        "a",
        "b",
        "c",
        "d",
        "e",
        "f",
        "h",
        "l",
        "h",
        "i",
        "r",
        "bc",
        "de",
        "hl",
        "sp",
        "ix",
        "iy",
        "ixl",
        "iyl",
    };

    for (auto name : prohibited_output_names)
        programm.AddOutputName(name);
}

void Compile(CParser &parser, CProgramm &programm, OutputFormat output_format, CString output_file_bin,
             CString asm_file_name) {
    Compiler compile(programm);
    compile.Compile(parser, output_format, output_file_bin, asm_file_name);
}

}  // namespace I8080
