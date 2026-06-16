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

#pragma once

#include "cmacronizer.h"

class CCondCompilation : public CMacroizer {
private:
    enum PreprocessorIgnoreMode {
        PIM_STOP_ON_ELIF_OR_ELSE,
        PIM_STOP_ON_ENDIF,
        PIM_STOP_ON_ENDIF_ELSE_PROCESSED,
    };
    bool PreprocessorIgnore(PreprocessorIgnoreMode mode);

public:
    bool preprocessor_mode{};
    std::function<void()> preprocessor;

    void NextToken();

    void PreprocessorEnter(size_t directive_line, size_t directive_column, const char *directive);
    bool PreprocessorIf(bool cond);
    bool PreprocessorElse(bool elif);
    bool PreprocessorEndIf();
    void PreprocessorSkipFile();
    void PreprocessorLeave();
};
