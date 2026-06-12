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

#include "index.h"
#include "../../c/tools/is8bitconst.h"
#include "../../c/tools/convert.h"

namespace I8080 {

static unsigned CanConvertTo8Bit(CNodePtr &node) {
    if (node->type == CNT_CONVERT) {
        switch (node->a->ctype.GetAsmType()) {
            case CBT_INT8:
                return IS8BITCONST_SIGNED;
            case CBT_UINT8:
                return IS8BITCONST_UNSIGNED;
        }
        return 0;
    }
    return Is8BitConst(node);
}

bool PrepareFast8BitOptimization(Prepare &, CNodePtr &node) {
    if (node->type == CNT_OPERATOR && !node->a->ctype.Is8BitType()) {
        switch (node->operator_code) {
            case COP_CMP_E:
            case COP_CMP_NE:
            case COP_CMP_LE:
            case COP_CMP_GE:
            case COP_CMP_L:
            case COP_CMP_G: {
                // Replace
                // CNT_OPERATOR(
                //   COP_CMP_*,
                //   CNT_CONVERT(any, 8 bit value) or CNT_NUMBER(8 bit value),
                //   CNT_CONVERT(any, 8 bit value) or CNT_NUMBER(8 bit value)
                // )
                // with
                // CNT_OPERATOR(
                //   COP_CMP_*,
                //   8 bit value or CNT_NUMBER(8 bit value),
                //   8 bit value or CNT_NUMBER(8 bit value)
                // )
                unsigned result = CanConvertTo8Bit(node->a);
                if (result) {
                    result &= CanConvertTo8Bit(node->b);  // Prevent: int8_t(-1) == int8_t(255)
                    if (result) {
                        node->a = Convert(CTYPE_UNSIGNED_CHAR, node->a);  // The sign has no meaning on 8 bits
                        node->b = Convert(CTYPE_UNSIGNED_CHAR, node->b);
                        return true;
                    }
                }
                return false;
            }
            case COP_AND:
            case COP_OR:
            case COP_XOR:
                // Replace
                // CNT_OPERATOR(
                //   COP_AND / COP_OR / COP_XOR,
                //   CNT_CONVERT(x, unsigned 8 bit value) / CNT_NUMBER(unsigned 8 bit value),
                //   CNT_CONVERT(x, unsigned 8 bit value) / CNT_NUMBER(unsigned 8 bit value)
                // )
                // with
                // CNT_CONVERT(x, CNT_OPERATOR(
                //   COP_AND / COP_OR / COP_XOR,
                //   unsigned 8 bit value / CNT_NUMBER(unsigned 8 bit value),
                //   unsigned 8 bit value / CNT_NUMBER(unsigned 8 bit value)
                // ))
                if (CanConvertTo8Bit(node->a) & IS8BITCONST_UNSIGNED) {
                    if (CanConvertTo8Bit(node->b) & IS8BITCONST_UNSIGNED) {
                        node->a = Convert(CTYPE_UNSIGNED_CHAR, node->a);
                        node->b = Convert(CTYPE_UNSIGNED_CHAR, node->b);
                        node = CNODE({CNT_CONVERT, a : node, ctype : node->ctype, e : node->e});
                        node->a->ctype = CTYPE_UNSIGNED_CHAR;
                        std::swap(node->next_node, node->a->next_node);
                        return true;
                    }
                }
                return false;
            case COP_MOD:
            case COP_DIV:
            case COP_SHR:
                // Replace
                // CNT_OPERATOR(
                //   COP_MOD / COP_DIV / COP_SHR,
                //   CNT_CONVERT(x, 8 bit value) / CNT_NUMBER(8 bit value),
                //   CNT_CONVERT(x, 8 bit value) / CNT_NUMBER(8 bit value)
                // )
                // with
                // CNT_CONVERT(x, CNT_OPERATOR(
                //   COP_MOD / COP_DIV / COP_SHR,
                //   8 bit value / CNT_NUMBER(8 bit value),
                //   8 bit value / CNT_NUMBER(8 bit value)
                // ))
                // TODO: check __o_div_u8, __o_div_i8, __o_mod_u8, __o_mod_i8...
                unsigned result = CanConvertTo8Bit(node->a);
                if (result) {
                    result &= CanConvertTo8Bit(node->b);
                    if (result) {
                        CType t{(result & IS8BITCONST_UNSIGNED) ? CTYPE_UNSIGNED_CHAR : CTYPE_SIGNED_CHAR};
                        node->a = Convert(t, node->a);
                        node->b = Convert(t, node->b);
                        node = CNODE({CNT_CONVERT, a : node, ctype : node->ctype, e : node->e});
                        node->a->ctype = t;
                        std::swap(node->next_node, node->a->next_node);
                        return true;
                    }
                }
                return false;
        }
    }
    return false;
}

}  // namespace I8080
