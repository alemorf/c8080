/*
 * c8080 stdlib
 * Copyright (c) 2026 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
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
#include <stddef.h>

#ifndef __STACK_SIZE
#define __STACK_SIZE 1024
#endif

#ifdef __C8080_NO_HEAP
#define __HEAP_INIT
#else

struct __HeapBlock {
    struct __HeapBlock *next;
    uint8_t free;
};

extern struct __HeapBlock __heap __address("__end");

// Description:
//  __heap.next = (void*)(SP - sizeof(struct __HeapBlock) - __STACK_SIZE);
//  __heap.next->next = NULL;
//  __heap.next->free = 0;
//  __heap.free = 1;

extern void *__heap_init_arg __address(sizeof(struct __HeapBlock) + __STACK_SIZE);

#define __HEAP_INIT                    \
    (void)&__heap;                     \
    (void)__heap_init_arg;             \
    asm("	ld hl, -__heap_init_arg"); \
    asm("	add hl, sp");              \
    asm("	ld (__heap), hl");         \
    asm("	xor a");                   \
    asm("	ld (hl), a");              \
    asm("	inc hl");                  \
    asm("	ld (hl), a");              \
    asm("	inc hl");                  \
    asm("	ld (hl), a");              \
    asm("	inc a");                   \
    asm("	ld (__heap + 2), a");
#endif
