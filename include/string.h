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
#include <stdint.h>
#include <c8080/c8080.h>

int8_t __global memcmp(const void *buffer1, const void *buffer2, size_t size) __link("string_h/memcmp.c");
void *__global memcpy(void *destination, const void *source, size_t size) __link("string_h/memcpy.c");
void *__global memmove(void *destination, const void *source, size_t size) __link("string_h/memmove.c");
void *__global memset(void *destination, uint8_t byte, size_t size) __link("string_h/memset.c");
void __global memswap(void *buffer1, void *buffer2, size_t size) __link("string_h/memswap.c");
char *__global strchr(const char *string, char byte) __link("string_h/strchr.c");
int8_t __global strcmp(const char *string1, const char *string2) __link("string_h/strcmp.c");
char *__global strcpy(char *destination, const char *source) __link("string_h/strcpy.c");
size_t __global strlen(const char *string) __link("string_h/strlen.c");
char *__global strcat(char *destination, const char *source) __link("string_h/strcat.c");
