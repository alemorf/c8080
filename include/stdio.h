/*
 * c8080 stdlib
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stddef.h>
#include <c8080/c8080.h>
#include <stdarg.h>

#define EOF (-1)

typedef struct FILE {
    int fd;
} FILE;

extern FILE *stdin __link("stdio_h/stdin.c");
extern FILE *stdout __link("stdio_h/stdout.c");
extern FILE *stderr __link("stdio_h/stderr.c");

int vsnprintf(char *buffer, size_t buffer_size, const char *format, va_list va) __link("stdio_h/snprintf.c");
int snprintf(char *buffer, size_t bufer_size, const char *format, ...) __link("stdio_h/snprintf.c");
int vsprintf(char *buffer, const char *format, va_list va) __link("stdio_h/snprintf.c");
int sprintf(char *buffer, const char *format, ...) __link("stdio_h/snprintf.c");
int printf(const char *format, ...) __link("stdio_h/printf.c");
int vprintf(const char *format, va_list va) __link("stdio_h/printf.c");
int puts(const char *text) __link("stdio_h/puts.c");
int getchar(void) __link("stdio_h/getchar.c");
int putchar(int c) __link("stdio_h/putchar.c");
