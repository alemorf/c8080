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

#include <stdint.h>
#include <c8080/c8080.h>

#define RAND_MAX 0x7FFF
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

typedef struct {
    char quot, rem;
} bdiv_t;

typedef struct {
    int quot, rem;
} div_t;

typedef struct {
    long int quot, rem;
} ldiv_t;

long strtol(const char *str, char **endptr, int base) __link("stdlib_h/strtoiul.c");
unsigned long strtoul(const char *str, char **endptr, int base) __link("stdlib_h/strtoiul.c");
void srand(unsigned seed) __link("stdlib_h/rand.c");
int rand(void) __link("stdlib_h/rand.c");
int abs(int x) __link("stdlib_h/abs.c");
long labs(long x) __link("stdlib_h/labs.c");
long long llabs(long long x) __link("stdlib_h/llabs.c");
