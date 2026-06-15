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

#pragma once

#include "../asm.h"

namespace I8080 {

bool UnrefLabel(AsmBase &a, AsmLabel *label);
AsmLabel *GetLabelDestinationRecursive(AsmBase &a, AsmLabel *label, AsmBase::Line *&out_line);
AsmBase::Line *SkipCommentsAndLabels(AsmBase &a, AsmBase::Line *line);
AsmBase::Line *GetNextLine(AsmBase &a, AsmBase::Line *line);
AsmBase::Line *GetNextLineSkipLabel(AsmBase &a, AsmBase::Line *line);

}  // namespace I8080
