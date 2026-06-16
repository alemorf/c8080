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

        if (token_data[0] != '#' || token_data[1] == '#' || !preprocessor)
            break;

        if (in_macro != 0) {
            CMacroizer::NextToken0AsString();
            break;
        }

        std::string directive;
        size_t line1 = token_line;
        size_t column1 = token_column;
        ReadDirective(directive);

        PreprocessorEnter(line1, column1 + 1, save_string(directive.c_str(), directive.size()));

        preprocessor();

        if (preprocessor_mode) {
            preprocessor_mode = false;
            Leave();
        }
    }
}

void CCondCompilation::PreprocessorEnter(size_t directive_line, size_t directive_column, const char *directive) {
    preprocessor_mode = true;
    enable_macro_in_preprocessor = false;
    Include(directive, file_name);
    line = directive_line;
    column = directive_column;
    NextToken2();
}

bool CCondCompilation::PreprocessorIgnore(PreprocessorIgnoreMode mode) {
    size_t level = 1;
    for (;;) {
        std::string line;
        size_t line_line = token_line;
        size_t line_column = token_column;
        if (!FindDirective(line)) {
            Error("unterminated #if");  // gcc
            return false;
        }

        if (line == "endif") {
            level--;
            if (level != 0)
                continue;
            assert(endif_counter != 0);
            endif_counter--;
            return false;
        }

        if (level == 1) {
            if (0 == line.compare(0, 4, "elif")) {
                if (mode == PIM_STOP_ON_ENDIF_ELSE_PROCESSED)
                    Error("#elif after #else");  // gcc
                if (mode != PIM_STOP_ON_ELIF_OR_ELSE)
                    continue;
                assert(endif_counter != 0);
                endif_counter--;
                PreprocessorEnter(line_line, line_column, save_string(line.c_str(), line.size()));
                return true;
            }
            if (line == "else") {
                if (mode == PIM_STOP_ON_ENDIF) {
                    mode = PIM_STOP_ON_ENDIF_ELSE_PROCESSED;
                    continue;
                }
                if (mode == PIM_STOP_ON_ENDIF_ELSE_PROCESSED)
                    Error("#else after #else");  // gcc
                assert(mode == PIM_STOP_ON_ELIF_OR_ELSE);
                return false;
            }
        }

        if (0 == line.compare(0, 2, "if"))
            level++;
    }
}

bool CCondCompilation::PreprocessorElse(bool elif) {
    PreprocessorLeave();
    if (endif_counter == 0)
        return false;
    PreprocessorIgnore(elif ? PIM_STOP_ON_ENDIF : PIM_STOP_ON_ENDIF_ELSE_PROCESSED);
    return true;
}

bool CCondCompilation::PreprocessorEndIf() {
    PreprocessorLeave();
    if (endif_counter == 0)
        return false;
    endif_counter--;
    return true;
}

bool CCondCompilation::PreprocessorIf(bool cond) {
    PreprocessorLeave();
    endif_counter++;
    if (!cond)
        return PreprocessorIgnore(PIM_STOP_ON_ELIF_OR_ELSE);
    return false;
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
