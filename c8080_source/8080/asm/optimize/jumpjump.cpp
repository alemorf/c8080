/*
 * c8080 compiler
 * Copyright (c) 2025 Aleksey Morozov
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
#include "common.h"

namespace I8080 {

// Replace
//     jp   ccc, label     jp   ccc, label    jp   ccc, label
//     jp   label2         ret                call label2
//     label:              label:             label:
// with
//     jp   ~ccc, label2   ret  ~ccc          call ~ccc, label2

static bool JumpOverJump(AsmBase &a, AsmBase::Line &l, AsmBase::Line *l1, AsmBase::Line *l2) {
    if (l1 && l2 && l.opcode == AC_JMP_CONDITION &&
        (l1->opcode == AC_JMP || l1->opcode == AC_RET || l1->opcode == AC_CALL) && l2->opcode == AC_LABEL &&
        l.argument[0].label != nullptr && l.argument[0].label == l2->argument[0].label) {
        UnrefLabel(a, l.argument[0].label);

        l.condition = InvertAsmCondition(l.condition);
        l.argument[0] = l1->argument[0];  // Replace the first argument

        switch (l1->opcode) {
            case AC_JMP:
                l.opcode = AC_JMP_CONDITION;
                break;
            case AC_RET:
                l.opcode = AC_RET_CONDITION;
                break;
            case AC_CALL:
                l.opcode = AC_CALL_CONDITION;
                break;
            default:
                assert(false);
        }

        *l1 = AsmBase::Line{AC_REMOVED};
        return true;
    }
    return false;
}

// Remove second line
//    xor/or/and/sub/add/cmp/adc/sbc r8
//    or a

static bool OraAfterAlu(AsmBase &a, AsmBase::Line &l, size_t i) {
    if (i > 0 && l.opcode == AC_ALU_REG && l.alu == ALU_OR && l.argument[0].reg == R8_A) {
        assert(l.argument[0].type == AAT_REG);
        AsmBase::Line &l_prev = a.lines[i - 1];
        if (l_prev.opcode == AC_ALU_REG || l_prev.opcode == AC_ALU_CONST) {
            l.opcode = AC_REMOVED;
            return true;
        }
    }
    return false;
}

static bool RemoveUnusedLabel(AsmBase &a, AsmBase::Line &l) {
    if (l.opcode == AC_LABEL && l.argument[0].label && l.argument[0].label->ref_count <= 1) {
        l.argument[0].label = NULL;
        l.opcode = AC_REMOVED;
        return true;
    }
    return false;
}

static bool JumpToJump(AsmBase &a, AsmBase::Line &l, AsmBase::Line *l1) {
    if (l.opcode == AC_JMP || l.opcode == AC_JMP_CONDITION) {
        // Replace
        //     jp  label1    jp  ccc, label1
        //     label1:       label1:
        // with
        //     label1:       label1:

        if (l1 && l1->opcode == AC_LABEL && l.argument[0] == l1->argument[0]) {
            if (l.argument[0].type == AAT_LABEL)
                UnrefLabel(a, l.argument[0].label);
            l.opcode = AC_REMOVED;
            l.argument[0].label = nullptr;
            return true;
        }

        if (l.argument[0].type == AAT_LABEL) {
            // Replace
            //     jp  label2    jp  ccc, label2
            //     ...           ...
            //     label2:       label2:
            //     ret           ret
            // with
            //     ret           ret  ccc
            //     ...           ...
            //     label2:       label2:
            //     ret           ret

            AsmLabel *&s = l.argument[0].label;
            AsmBase::Line *l2 = nullptr;
            AsmLabel *d = GetLabelDestinationRecursive(a, s, l2);
            if (l2->opcode == AC_RET) {
                UnrefLabel(a, l.argument[0].label);
                l.opcode = (l.opcode == AC_JMP) ? AC_RET : AC_RET_CONDITION;
                l.argument[0].label = nullptr;
                return true;
            }

            // Replace
            //     jp  label2    jp  ccc, label2
            //     ...           ...
            //     label2:       label2:
            //     jp  label3    jp  label3
            // with
            //     jp  label3    jp  ccc, label3
            //     ...           ...
            //     label2:       label2:
            //     jp  label3    jp  label3

            if (s != d) {
                d->ref_count++;
                UnrefLabel(a, s);
                s = d;
                return true;
            }
        }
    }
    return false;
}

static bool LastCall(AsmBase &a, AsmBase::Line &l) {
    // Replace
    //     call function     call cc, function
    //     some_label:       some_label:
    //     ret               ret
    // with
    //     jp   function     jp   cc, function
    //     some_label:       some_label:
    //     ret               ret

    if (l.opcode == AC_CALL || l.opcode == AC_CALL_CONDITION) {
        AsmBase::Line *l2 = GetNextLineSkipLabel(a, &l);
        if (l2 && l2->opcode == AC_RET) {
            l.opcode = (l.opcode == AC_CALL) ? AC_JMP : AC_JMP_CONDITION;
            return true;
        }
    }
    return false;
}

static bool JumpJump(AsmBase &a, AsmBase::Line &l, size_t i) {
    AsmBase::Line *l1 = GetNextLine(a, &l);
    AsmBase::Line *l2 = GetNextLine(a, l1);

    if (JumpOverJump(a, l, l1, l2))
        return true;
    if (OraAfterAlu(a, l, i))
        return true;
    if (RemoveUnusedLabel(a, l))
        return true;
    if (JumpToJump(a, l, l1))
        return true;
    if (LastCall(a, l))
        return true;

    return false;
}

bool AsmOptimizeJumpJump(AsmBase &a) {
    bool changed = false;
    bool restart;
    do {
        restart = false;
        size_t i = 0;
        for (auto &l : a.lines) {
            while (JumpJump(a, l, i)) {
                restart = true;
                changed = true;
            }
            i++;
        }
    } while (restart);
    return changed;
}

}  // namespace I8080
