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

#include "../cprogramm.h"
#include "../../tools/cstring.h"
#include <set>

class CParser {
public:
    CProgramm &programm;
    std::vector<std::string> include_dirs;
    std::vector<std::string> default_defines;
    std::map<uint32_t, uint8_t> codepage;

    CParser(CProgramm &p) : programm(p) {
    }
    void AddSourceFile(CString file_name);
    CString GetFirstSourceFile(std::string &out_file_name, const char *default_name);
    void ParseAll();
    const char *LoadFile(CString file_name, const char **out_file_name = nullptr);
    bool FindGlobalIncludeFile(CString file_name, std::string &result);
    bool FindAnyIncludeFile(CString file_name, CString local_path, std::string &result);
    const char *LoadGlobalIncludeFile(CString file_name);

private:
    std::set<CString> compile_queue_index;
    std::vector<CString> compile_queue;
    std::map<std::string, std::string> loaded_files;  // TODO: Перенести в programm
};
