/*
 * c8080 compiler
 * Copyright (c) 2026 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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

#include "Compiler.h"
#include "../../c/tools/getnumberasuint64.h"

namespace I8080 {

void Compiler::OutShl8(CNodePtr &node, AsmRegister reg) {
    assert(node->type == CNT_OPERATOR);
    assert(node->operator_code == COP_SHL);
    assert(node->a != nullptr);
    assert(node->b != nullptr);
    assert(node->a->ctype.Is8BitType());
    assert(node->b->type == CNT_NUMBER);
    assert(reg == R8_A || reg == REG_NONE);

    if (reg == REG_NONE) {
        Build(node->a, REG_NONE);
        return;
    }

    uint8_t value = GetNumberAsUint64(node->b);

    if (value >= 8) {
        Build(node->a, REG_NONE);
        out.ld_a_n8(0);  // This is XOR
        return;
    }

    Build(node->a, R8_A);

    if (value == 7) {
        out.cyclic_rotate_right();
        out.and_number(0x80);
        return;
    }

    if (value == 6) {
        out.cyclic_rotate_right();
        out.cyclic_rotate_right();
        out.and_number(0xC0);
        return;
    }

    for (unsigned i = 0; i < value; i++)
        out.add_a();
}

}  // namespace I8080
