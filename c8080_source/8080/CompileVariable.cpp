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

#include "CompileVariable.h"
#include "prepare/prepare.h"

namespace I8080 {

void CompileVariable(Asm &out, CProgramm &p, CVariablePtr &vd) {
    assert(!vd->address_attribute.Exists());

    PrepareVariable(p, vd, out);

    if (vd->align_attribute.Exists())
        out.align(vd->align_attribute.Get());

    out.variable(vd->output_name);

    size_t bytes_written = 0;
    for (CNodePtr j = vd->body; j != nullptr; j = j->next_node) {
        if (j->type == CNT_IMMEDIATE_STRING) {
            std::shared_ptr<CConstString> &const_string = j->const_string;
            if (const_string != nullptr) {
                assert(j->ctype.pointers.size() == 1 && j->ctype.pointers[0].is_array);
                const size_t l = j->ctype.pointers[0].array_size;
                const size_t s = std::min(l, const_string->text.size());
                for (size_t i = 0; i < s; i++)
                    out.db(const_string->text[i]);
                out.ds(l - s);
                bytes_written += l;
            } else {
                out.ds(j->ctype.SizeOf(j->e));
                bytes_written += j->ctype.SizeOf(j->e);
            }
        } else if (j->IsConstNode()) {
            bytes_written += out.data(j);
        } else {
            p.Error(j->e, "initializer element is not constant");  // gcc
        }
    }
    out.ds(vd->type.SizeOf(vd->e) - bytes_written);
}

}  // namespace I8080
