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

#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <filesystem>
#include "tools/catpath.h"
#include "tools/getpath.h"
#include "tools/removeextension.h"
#include "8080/Compile.h"
#include "c/tools/dump.h"
#include "c/parser/cparser.h"
#include "8080/cmm/prepare.h"
#include "prepare/prepare.h"
#include "8080/cmm/cmm.h"
#include "8080/cmm/names.h"

#ifdef __MINGW32__
static const char *ASSEMBLER = "sjasmplus.exe";
#elif __APPLE__
static const char *ASSEMBLER = "sjasmplus.macos";
#else
static const char *ASSEMBLER = "sjasmplus";
#endif

struct Options {
    I8080::OutputFormat output_format = I8080::OF_CPM;  // -O
    std::string bin_file_name;                          // -o
    std::string asm_file_name;                          // -a
    bool print_expression_tree = false;                 // -V
    bool print_expression_tree_opt = false;             // -W
    bool assembler_need_path = true;                    // -A
    std::string assembler = ASSEMBLER;                  // -A
};

static void Usage(char **argv) {
    std::cout << "Usage: " << argv[0] << " [options] file1.c file2.c ..." << std::endl
              << "Options:" << std::endl
              << "  -m         Compile CMM language" << std::endl
              << "  -I<path>   Add include directory" << std::endl
              << "  -Ocpm      Make binary file for CP/M" << std::endl
              << "  -Oi1080    Make binary file for Iskra 1080 Tartu" << std::endl
              << "  -Orks      Make binary file for Specialist" << std::endl
              << "  -Da        Set #define a" << std::endl
              << "  -Da=b      Set #define a b" << std::endl
              << "  -Da(b)c    Set #define a(b) c" << std::endl
              << "  -o<file>   Set name for output binary file" << std::endl
              << "  -a<file>   Set name for output assembler file" << std::endl
              << "  -A<file>   Alternative assembler tool" << std::endl
              << "  -S         __stack by default" << std::endl
              << "  -G         __global by default" << std::endl
              << "  -V         Print expression tree after parsing" << std::endl
              << "  -W         Print expression tree after compilation" << std::endl
              << "  --         Last option" << std::endl;
}

static void ParseOptions(int argc, char **argv, Options &o, CParser &c) {
    bool disable_options = false;
    for (int i = 1; i < argc; i++) {
        char *s = argv[i];
        if (s[0] == '-' && !disable_options) {
            if (s[1] != 0 && s[2] == 0) {
                switch (s[1]) {
                    case 'V':
                        o.print_expression_tree = true;
                        continue;
                    case 'W':
                        o.print_expression_tree_opt = true;
                        continue;
                    case '-':
                        disable_options = true;
                        continue;
                    case 'm':
                        c.programm.cmm = true;
                        continue;
                    case 'S':
                        c.programm.default_variables_mode = CVM_STACK;
                        continue;
                    case 'G':
                        c.programm.default_variables_mode = CVM_GLOBAL;
                        continue;
                }
            }
            const char *value = s[1] != 0 ? s + 2 : "";
            if (value[0] == 0) {
                if (i + 1 >= argc)
                    throw std::runtime_error(std::string("missing value after '") + s + "'");  // gcc
                i++;
                value = argv[i];
            }
            switch (s[1]) {
                case 'I':
                    c.include_dirs.push_back(value);
                    continue;
                case 'O':
                    if (I8080::ParseOutputFormat(o.output_format, value))
                        continue;
                    break;
                case 'D': {
                    static const char ids[] =
                        "abcdefghijklmnopqrstuvwxyz"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "0123456789_";
                    std::string v = std::string("#define ") + value;
                    auto p = v.find_first_not_of(ids, 8);
                    if (p != std::string::npos && v[p] == '=')
                        v[p] = ' ';
                    c.default_defines.push_back(v);
                    continue;
                }
                case 'o':
                    o.bin_file_name = value;
                    continue;
                case 'a':
                    o.asm_file_name = value;
                    continue;
                case 'A':
                    o.assembler = value;
                    o.assembler_need_path = false;
                    continue;
            }
            throw std::runtime_error("unrecognized command-line option '" + std::string(s) + "'");  // gcc
        }
        c.AddSourceFile(s);
    }
}

static int BadExit(const char *text = nullptr) {
    if (text)
        std::cerr << text << std::endl;
    std::cerr << "Compilation terminated due to error" << std::endl;
    return 1;
}

static std::string ToLowerCase(std::string str) {
    for (char &c : str)
        c = tolower(c);
    return str;
}

struct RksHeader {
    uint16_t start_le16;
    uint16_t stop_le16;
};

struct RksFooter {
    uint16_t crc_le16;
};

static void MakeRKS(CString file_name) {
    std::vector<uint8_t> data;
    FsTools::LoadFile(file_name, UINT16_MAX, data);

    if (data.empty())
        return;

    uint16_t checksum = 0;
    for (size_t i = 0; i < data.size() - 1; i++)
        checksum += data[i] * 257;
    checksum = (checksum & 0xFF00) + ((checksum + data.back()) & 0xFF);

    uint16_t end = data.size() - 1;
    uint8_t header[4] = {0, 0, uint8_t(end), uint8_t(end >> 8)};
    data.insert(data.begin(), header, header + sizeof(header));

    uint8_t footer[2] = {uint8_t(checksum), uint8_t(checksum >> 8)};
    data.insert(data.end(), footer, footer + sizeof(footer));

    FsTools::SaveFile(file_name, data);
}

int main(int argc, char **argv) {
    try {
        std::cout << "C Compiler for i8080 (" __DATE__ << ")" << std::endl
                  << "(c) Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru" << std::endl;

        if (argc < 2) {
            Usage(argv);
            return 0;
        }

        CProgramm programm;
        CParser c(programm);

        std::string std_include_dir = CatPath(GetPath(argv[0]), "include");
        if (std::filesystem::is_directory(std_include_dir))
            c.include_dirs.push_back(std_include_dir);

        Options o;
        ParseOptions(argc, argv, o, c);

        if (o.bin_file_name.empty() || o.asm_file_name.empty()) {
            std::string base_name;
            if (!o.bin_file_name.empty())
                base_name = o.bin_file_name;
            else if (!c.GetFirstSourceFile(base_name))
                base_name = "a";
            base_name = RemoveExtension(base_name);

            if (o.asm_file_name.empty())
                o.asm_file_name = base_name + ".asm";
            if (o.bin_file_name.empty())
                o.bin_file_name = base_name + ".bin";
        }

        std::string arch_inc1lude_dir = CatPath(std_include_dir, "arch");
        for (auto &i : c.default_defines) {
            static const char prefix[] = "#define ARCH_";
            if (0 == strncmp(i.c_str(), prefix, sizeof(prefix) - 1)) {
                std::string dir = CatPath(arch_inc1lude_dir, ToLowerCase(i.substr(sizeof(prefix) - 1)));
                if (std::filesystem::is_directory(dir))
                    c.include_dirs.push_back(dir);
            }
        }

        if (o.assembler_need_path) {
            std::string assembler1 = CatPath(GetPath(argv[0]), o.assembler);
            if (std::filesystem::is_regular_file(assembler1)) {
                o.assembler = assembler1;
            } else if (!c.include_dirs.empty()) {
                o.assembler = CatPath(c.include_dirs[0], o.assembler);
            }
        }

        std::remove(o.asm_file_name.c_str());
        std::remove(o.bin_file_name.c_str());

        if (programm.cmm) {
            I8080::RegisterInternalCmmNames(programm);
            c.default_defines.push_back("#define __CMM");
        } else {
            I8080::RegisterProhibitedOutputNames(programm);

            std::string internal_c_file_name;
            if (!c.FindGlobalIncludeFile("c8080/internal.c", internal_c_file_name))
                throw std::runtime_error("file \"c8080/internal.c\" not found");
            c.AddSourceFile(internal_c_file_name);
        }

        c.ParseAll();

        if (programm.error)
            return 1;

        if (o.print_expression_tree)
            Dump(programm.first_node, "");

        if (programm.cmm) {
            I8080::CompileCmm(programm, o.asm_file_name);
        } else {
            I8080::Compile(c, programm, o.output_format, o.bin_file_name, o.asm_file_name);
        }

        if (o.print_expression_tree_opt)
            Dump(programm.first_node, "");

        if (programm.error)
            return BadExit();

        std::string lst_file_name = RemoveExtension(o.asm_file_name) + ".lst";

        std::string asm_cmd_line = o.assembler + " " + o.asm_file_name + " --lst=" + lst_file_name;
        int r = system(asm_cmd_line.c_str());
        if (r != 0)
            throw std::runtime_error("Assembler error " + std::to_string(r) + " (" + asm_cmd_line + ")");

        if (o.output_format == I8080::OF_RKS)
            MakeRKS(o.bin_file_name);

        std::cout << "Done" << std::endl;
        return 0;
    } catch (std::exception &e) {
        return BadExit(e.what());
    }
}
