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

#include "ccondcompilation.h"

void CCondCompilation::NextToken() {
    if (preprocessor_mode) {
        if (enable_macro_in_preprocessor) {
            CMacroizer::NextToken();
            return;
        }
        NextToken2();
        return;
    }
    for (;;) {
        CMacroizer::NextToken();

        if (token_data[0] != '#' || !preprocessor)
            break;

        if (in_macro != 0)
            Throw("# in macro");  // TODO

        preprocessor_mode = true;
        enable_macro_in_preprocessor = false;
        std::string directive;
        size_t line1 = token_line;
        size_t column1 = token_column;
        ReadDirective(directive);
        Include(save_string(directive.c_str(), directive.size()), file_name);
        line = line1;
        column = column1 + 1;
        NextToken2();

        preprocessor();

        if (preprocessor_mode) {
            preprocessor_mode = false;
            Leave();
        }
    }
}

void CCondCompilation::PreprocessorIgnore(bool can_else) {
    size_t level = 1;
    for (;;) {
        std::string line;
        if (!FindDirective(line)) {
            Error("unterminated #if");  // gcc
            break;
        }

        if (line == "endif") {
            level--;
            if (level != 0)
                continue;
            assert(endif_counter != 0);
            endif_counter--;
            break;
        }

        if (level == 1 && line == "else") {
            if (!can_else)
                Error("#else after #else");  // gcc
            break;
        }

        if (0 == line.compare(0, 2, "if"))
            level++;
    }
}

bool CCondCompilation::PreprocessorElse() {
    PreprocessorLeave();
    if (endif_counter == 0)
        return false;
    PreprocessorIgnore(false);
    return true;
}

bool CCondCompilation::PreprocessorEndIf() {
    PreprocessorLeave();
    if (endif_counter == 0)
        return false;
    endif_counter--;
    return true;
}

void CCondCompilation::PreprocessorIf(bool cond) {
    PreprocessorLeave();
    endif_counter++;
    if (!cond)
        PreprocessorIgnore(true);
}

void CCondCompilation::PreprocessorSkipFile() {
    PreprocessorLeave();
    if (!Leave())
        cursor += strlen(cursor);
}

void CCondCompilation::PreprocessorLeave() {
    if (preprocessor_mode) {
        preprocessor_mode = false;
        Leave();
    }
}
