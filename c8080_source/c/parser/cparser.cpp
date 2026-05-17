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

#include "cparser.h"
#include <filesystem>
#include "cparserfile.h"
#include "../cnodelist.h"
#include "../../tools/fs_tools.h"
#include "../../tools/catpath.h"
#include "../../tools/getpath.h"

static bool FindIncludeFile(CString path, CString base_name, std::string &result) {
    result = CatPath(path, base_name);
    return std::filesystem::is_regular_file(result);
}

bool CParser::GetFirstSourceFile(std::string &out_file_name) {
    if (compile_queue.empty())
        return false;
    out_file_name = compile_queue[0];
    return true;
}

bool CParser::FindAnyIncludeFile(CString file_name, CString local_path, std::string &result) {
    if (FindIncludeFile(GetPath(local_path), file_name, result))
        return true;
    return FindGlobalIncludeFile(file_name, result);
}

bool CParser::FindGlobalIncludeFile(CString file_name, std::string &result) {
    for (auto &dir : include_dirs)
        if (FindIncludeFile(dir, file_name, result))
            return true;
    return false;
}

const char *CParser::LoadGlobalIncludeFile(CString file_name) {
    std::string full_file_name;
    if (!FindGlobalIncludeFile(file_name, full_file_name))
        throw std::runtime_error(std::string("file \"") + file_name + "\" not found");
    return LoadFile(full_file_name);
}

void CParser::AddSourceFile(CString file_name) {
    if (compile_queue_index.find(file_name) == compile_queue_index.end()) {
        compile_queue_index[file_name] = 1;
        compile_queue.push_back(file_name);
    }
}

const char *CParser::LoadFile(CString file_name, const char **out_file_name) {
    auto i = loaded_files.find(file_name);
    if (i == loaded_files.end()) {
        std::string &contents = loaded_files[file_name];
        FsTools::LoadFile(file_name, SIZE_MAX, contents);

        // Remove UTF8 header
        static const uint8_t utf8_header[3] = {0xEF, 0xBB, 0xBF};
        if (contents.size() >= sizeof(utf8_header) && 0 == memcmp(contents.data(), utf8_header, sizeof(utf8_header)))
            contents.erase(0, sizeof(utf8_header));

        i = loaded_files.find(file_name);
        assert(i != loaded_files.end());
    }
    if (out_file_name != nullptr)
        *out_file_name = i->first.c_str();
    return i->second.c_str();
}

void CParser::ParseAll() {
    CNodeList list;
    list.PushBack(programm.first_node);
    for (size_t i = 0; i < compile_queue.size(); i++) {
        std::string name_copy = compile_queue[i];  // vector<string> compile_queue can be chagned
        CParserFile c(*this);
        c.Parse(list, name_copy.c_str());
    }
    programm.first_node = list.first;
}
