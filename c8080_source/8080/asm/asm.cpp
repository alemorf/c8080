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

#include "asm.h"
#include "../../c/tools/cthrow.h"

namespace I8080 {

std::string Asm::GetConst(const CNodePtr &node, bool *error, std::vector<CVariablePtr> *use) {
    switch (node->type) {
        // TODO: Replace CNT_CONST_STRING with CNT_LOAD_VARIABLE
        case CNT_CONST_STRING:
            if (node->const_string == nullptr)
                C_ERROR_INTERNAL(node->e, "null pointer");
            if (!measure)
                return node->const_string->compiler.GetName(const_string_counter);
            return "measure";
        case CNT_NUMBER:
            switch (node->ctype.GetAsmType()) {
                case CBT_INT8:
                    return std::to_string(int8_t(node->number.i));
                case CBT_INT16:
                    return std::to_string(int16_t(node->number.i));
                case CBT_INT32:
                    return std::to_string(int32_t(node->number.i));
                case CBT_INT64:
                    return std::to_string(node->number.i);
                case CBT_UINT8:
                case CBT_UINT16:
                case CBT_UINT32:
                case CBT_UINT64:
                    return std::to_string(node->number.u);
                case CBT_FLOAT:
                    return std::to_string(node->number.f);
                case CBT_DOUBLE:
                    return std::to_string(node->number.d);
                case CBT_LONG_DOUBLE:
                    return std::to_string(node->number.ld);
                default:  // TODO: other types
                    C_ERROR_UNSUPPORTED_ASM_TYPE(node);
            }
            break;
        case CNT_CONST:
            if (use != nullptr) {
                assert(!measure);
                use->insert(use->begin(), node->compiler.used_variables.begin(), node->compiler.used_variables.end());
            } else if (!measure) {
                for (auto &i : node->compiler.used_variables) {
                    if (i->c.use_counter == 0) {
                        i->c.use_counter++;
                        if (i->type.IsFunction() && !i->only_extern)
                            compile_queue.push_back(i);
                    }
                }
            }
            assert(!node->text.empty());
            return node->text;
    }
    if (error)
        *error = true;
    else
        p.Error(node->e, "only constant");  // TODO: programm->error
    return "0";
}

}  // namespace I8080
