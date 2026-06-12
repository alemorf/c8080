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

#include <string>

enum CNodeType {
    CNT_NUMBER,              // Example: 12
    CNT_LOAD_VARIABLE,       // Example: a
    CNT_RETURN,              // Example: return [a]
    CNT_IF,                  // Example: if (a) b [ else c ]
    CNT_DO,                  // Example: do { a } while (b)
    CNT_WHILE,               // Example: while (a) b
    CNT_BREAK,               // Example: break
    CNT_CONTINUE,            // Example: continue
    CNT_SWITCH,              // Example: switch(a) { b }
    CNT_CASE,                // Example: case a:
    CNT_DEFAULT,             // Example: default:
    CNT_LEVEL,               // Example: { a }
    CNT_DECLARE_VARIABLE,    // Example: int a [= b];
    CNT_OPERATOR,            // Example: a + b
    CNT_MONO_OPERATOR,       // Example: -a
    CNT_SIZEOF_TYPE,         // Example: sizeof(a)
    CNT_FUNCTION_CALL,       // Example: function(a)
    CNT_FUNCTION_CALL_ADDR,  // Example: pointer(b);
    CNT_TYPEDEF,             // Example: typedef a;
    CNT_CONST_STRING,        // Example: "a"
    CNT_FOR,                 // Example: for(a; b; c) d
    CNT_CONVERT,             // Example: (int)a
    CNT_PUSH_POP,            // Example: push_pop(a) { b }  Only in CMM
    CNT_ASM,                 // Example: asm { a }
    CNT_LABEL,               // Example: a:
    CNT_GOTO,                // Example: goto a
    CNT_IMMEDIATE_STRING,    // Example: const char str[4] = "HELP";

    // Optimizer replaces CNT_LOAD_VARIABLE with
    CNT_CONST,               // Expression for asm file
    CNT_SAVE_TO_REGISTER,    // The processor register in which the last argument to the function is passed
    CNT_LOAD_FROM_REGISTER,  // The processor register in which the last argument to the function is passed
    CNT_STACK_ADDRESS,       // Address relative to the first stack variable of the function
    CNT_ARG_STACK_ADDRESS,   // Address relative to the first argument of the function
};

std::string ToString(CNodeType type);
