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

#include "Compiler.h"

namespace I8080 {

void Compiler::MeasureArgsBegin() {
    out.measure_metric = 0;
    out.measure_regs = 0;
}

void Compiler::MeasureArgsEnd(unsigned id) {
    if (out.measure_args_id == 0 || out.measure_args_metric > out.measure_metric) {
        out.measure_args_id = id;
        out.measure_args_metric = out.measure_metric;
        out.measure_args_regs = out.measure_regs;
    }
}

void Compiler::Measure(CNodePtr &node, AsmRegister reg, CBuildProc proc) {
    MeasureBegin();
    measure_proc = proc;
    if ((this->*proc)(node, reg))
        MeasureResult(node, reg);
    out.measure = false;
    measure_proc = nullptr;
}

void Compiler::MeasureBegin(AsmRegister reg) {
    if (reg == REG_PREPARE)
        MeasureBegin();
}

void Compiler::MeasureBegin() {
    assert(!out.measure);
    out.measure = true;
    out.measure_metric = 0;
    out.measure_regs = 0;
    out.measure_args_metric = 0;
    out.measure_args_id = 0;
}

void Compiler::MeasureResult(CNodePtr &node, AsmRegister reg) {
    if (out.measure) {
        if (reg != REG_NONE) {
            CBuildCase &c = node->compiler.Get(reg);
            if (!c.able || c.metric > out.measure_metric)
                c.Set(out.measure_regs, out.measure_metric, out.measure_args_id, measure_proc);
        }
        CBuildCase &n = node->compiler.no_result;
        if (!n.able || n.metric > out.measure_metric)
            n.Set(out.measure_regs, out.measure_metric, out.measure_args_id, measure_proc);
    }
}

bool Compiler::MeasureReset(CNodePtr &node, AsmRegister reg) {
    if (reg == REG_PREPARE) {
        node->compiler.no_result.Reset();
        node->compiler.main.Reset();
        node->compiler.alt.Reset();
        return false;
    }

    const CBuildCase &c = node->compiler.Get(reg);
    if (c.build == nullptr)
        C_ERROR_INTERNAL(node, "No compilation cases");

    if (out.measure) {
        out.measure_metric += c.metric;
        out.measure_regs |= c.regs;
        return true;
    }

    (this->*(c.build))(node, reg);
    return true;
}

}  // namespace I8080
