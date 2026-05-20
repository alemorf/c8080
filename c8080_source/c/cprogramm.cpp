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

#include "cprogramm.h"
#include <stdexcept>
#include <iostream>
#include "../tools/listext.h"

bool CProgramm::AddOutputName(CString name) {
    output_names[name] = 1;
    return true;
}

CVariablePtr CProgramm::FindVariable(CString name) {
    auto i = variables.find(name);
    return (i == variables.end()) ? nullptr : i->second;
}

void CProgramm::AddVariable(CVariablePtr a) {
    static const size_t max_string_size = 32;

    std::string lo_name = a->output_name;
    if (lo_name.size() > max_string_size)
        lo_name.resize(max_string_size);

    for (char &c : lo_name)
        c = tolower(c);

    std::string alt_name = lo_name;
    if (!cmm || !a->is_label)
        while (output_names.find(alt_name) != output_names.end())
            alt_name = lo_name + "_" + std::to_string(unique_counter++);
    a->output_name = alt_name;

    output_names[alt_name] = 0;

    all_top_variables.push_back(a);

    if (a->name.empty())
        a->name = "?" + a->output_name;
}

CConstStringPtr CProgramm::RegisterConstString(CString text) {
    auto i = const_strings.find(text);
    if (i != const_strings.end())
        return i->second;

    CConstStringPtr const_string = std::make_shared<CConstString>();
    const_strings[text] = const_string;
    const_string->text = text;
    return const_string;
}

void CProgramm::Error(const CErrorPosition &e, CString text, const char *type) {
    std::string full_text = e.ToString() + ": " + type + ": " + text;
    if (last_error != full_text) {
        std::cerr << full_text << std::endl;
        last_error = full_text;
    }
    if (type[0] == 'e')
        error = true;
}

void CProgramm::Note(const CErrorPosition &e, CString text) {
    Error(e, text, "note");
}

const char *CProgramm::SaveString(const char *data, size_t size) {
    std::string &buffer = *Add(saved_strings);  // TODO: Big buffer
    buffer.assign(data, size);
    return buffer.c_str();
}
