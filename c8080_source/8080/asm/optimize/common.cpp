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

#include "common.h"
#include <stdexcept>

namespace I8080 {

bool UnrefLabel(AsmBase &a, AsmLabel *label) {
    assert(label != nullptr);

    label->ref_count--;
    if (label->ref_count > 1)  // labels also increase ref_count
        return false;

    // Now the label points only to itself and can be deleted

    if (label->destination <= 0 || label->destination > a.lines.size())
        throw std::runtime_error(std::string("Internal error 1 in ") + __PRETTY_FUNCTION__);

    label->ref_count = 0;
    AsmBase::Line &labelLine = a.lines[label->destination - 1];

    if (labelLine.opcode != AC_LABEL)
        throw std::runtime_error(std::string("Internal error 2 in ") + __PRETTY_FUNCTION__);

    labelLine.opcode = AC_REMOVED;
    labelLine.argument[0].label = nullptr;
    return true;
}

AsmLabel *GetLabelDestinationRecursive(AsmBase &a, AsmLabel *label, AsmBase::Line *&out_line) {
    std::vector<AsmLabel *> path;
    for (;;) {
        // Prevent segmentation fault when internal structure is damaged
        if (label == nullptr || label->destination >= a.lines.size())
            throw std::runtime_error(std::string("Internal error in ") + __PRETTY_FUNCTION__);

        // Skip unnecessary lines
        AsmBase::Line *line = &a.lines[label->destination];
        line = SkipCommentsAndLabels(a, line);

        // Detect jump to jump
        if (line->opcode == AC_JMP && line->argument[0].label) {
            // Prevent infinite loop
            if (std::find(path.begin(), path.end(), label) == path.end()) {
                path.push_back(label);
                label = line->argument[0].label;
                continue;
            }
        }

        out_line = line;
        return label;
    }
}

AsmBase::Line *SkipCommentsAndLabels(AsmBase &a, AsmBase::Line *line) {
    assert(line != nullptr);
    AsmBase::Line *const lines_end = &*a.lines.end();
    while (line + 1 < lines_end && (line->opcode == AC_REMOVED || line->opcode == AC_LINE ||
                                    line->opcode == AC_REMARK || line->opcode == AC_LABEL))
        line++;
    return line;
}

AsmBase::Line *GetNextLine(AsmBase &a, AsmBase::Line *line) {
    if (line == nullptr)
        return nullptr;
    AsmBase::Line *const lines_end = &*a.lines.end();
    for (;;) {
        line++;
        if (line == lines_end)
            return nullptr;
        if (line->opcode == AC_REMOVED || line->opcode == AC_LINE || line->opcode == AC_REMARK)
            continue;
        return line;
    }
}

AsmBase::Line *GetNextLineSkipLabel(AsmBase &a, AsmBase::Line *line) {
    if (line == nullptr)
        return nullptr;
    AsmBase::Line *const lines_end = &*a.lines.end();
    for (;;) {
        line++;
        if (line == lines_end)
            return nullptr;
        if (line->opcode == AC_REMOVED || line->opcode == AC_LINE || line->opcode == AC_REMARK ||
            line->opcode == AC_LABEL)
            continue;
        return line;
    }
}

};  // namespace I8080
